#include "math/vec4.h"
#include "renderer.h"

#include <cstdint>

#include <algorithm>

Renderer::Renderer()
    : m_width(0),
      m_height(0)
{}

Renderer::Renderer(uint32_t width, uint32_t height)
    : m_width(width),
      m_height(height)
{
    m_frame_buffer.resize(width * height, Vec4(0.0, 0.0, 0.0, 1.0));
    m_depth_buffer.resize(width * height, std::numeric_limits<float>::max());
}

void Renderer::clearBuffers()
{
    std::fill(m_frame_buffer.begin(), m_frame_buffer.end(), Vec4(0.0, 0.0, 0.0, 1.0));
    std::fill(m_depth_buffer.begin(), m_depth_buffer.end(), std::numeric_limits<float>::max());
}

void Renderer::render(const FrameData& frameData)
{
    setProjectionMatrix(frameData.camera.getProjectionMatrix());
    setViewMatrix(frameData.camera.getViewMatrix());
    for (const auto& mesh_data : frameData.meshes) {
        renderMesh(mesh_data.mesh, mesh_data.transform);
    }
}

void Renderer::renderMesh(const Mesh* mesh, const Mat4& transform)
{
    if (mesh == nullptr) {
        return;
    }

    auto& vertices = mesh->getVertices();
    auto& indices = mesh->getIndices();
    m_model_matrix = transform;
    for (size_t i = 0; i < indices.size(); i += 3) {
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];

        renderTriangle(v0, v1, v2);
    }
}

bool Renderer::transformVertex(const Vec3& worldPos, Vec3& outScreen) const
{
    Vec4 clip_pos = m_proj_matrix * m_view_matrix * m_model_matrix * Vec4(worldPos, 1.0f);

    // Cull vertices behind or exactly at the near plane
    if (clip_pos.w() <= 1e-5f) {
        return false;
    }

    float invW = 1.0f / clip_pos.w();

    // Perspective divide -> NDC [-1, 1]
    float ndcX = clip_pos.x() * invW;
    float ndcY = clip_pos.y() * invW;
    float ndcZ = clip_pos.z() * invW;

    // Viewport transform -> screen pixels
    float screenX = (ndcX * 0.5f + 0.5f) * static_cast<float>(m_width);
    float screenY = (1 - (ndcY * 0.5f + 0.5f)) * static_cast<float>(m_height);

    outScreen = Vec3(screenX, screenY, ndcZ);
    return true;
}

void Renderer::renderTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
    // Transform world-space positions to screen space
    Vec3 t0, t1, t2;
    if (!transformVertex(v0.position, t0) || !transformVertex(v1.position, t1) || !transformVertex(v2.position, t2)) {
        return;
    }

    Vec2 p0(t0.x(), t0.y());
    Vec2 p1(t1.x(), t1.y());
    Vec2 p2(t2.x(), t2.y());

    int minX = static_cast<int>(std::min({t0.x(), t1.x(), t2.x()}));
    int maxX = static_cast<int>(std::max({t0.x(), t1.x(), t2.x()}));
    int minY = static_cast<int>(std::min({t0.y(), t1.y(), t2.y()}));
    int maxY = static_cast<int>(std::max({t0.y(), t1.y(), t2.y()}));

    // Clamp to viewport
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, static_cast<int>(m_width) - 1);
    maxY = std::min(maxY, static_cast<int>(m_height) - 1);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Vec2 pixelPos(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
            Vec3 barycentric = computeBarycentric(pixelPos, p0, p1, p2);

            const float alpha = barycentric.x();
            const float beta = barycentric.y();
            const float gamma = barycentric.z();

            constexpr float epsilon = 1e-6f;
            if (alpha < -epsilon || beta < -epsilon || gamma < -epsilon) {
                continue;
            }

            // Interpolate NDC depth and vertex color
            float depth = t0.z() * alpha + t1.z() * beta + t2.z() * gamma;
            Vec4 color = v0.color * alpha + v1.color * beta + v2.color * gamma;

            setPixel(static_cast<uint32_t>(x), static_cast<uint32_t>(y), color, depth);
        }
    }
}

Vec3 Renderer::computeBarycentric(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2)
{
    float area2 = cross(p1 - p0, p2 - p0);

    float alpha = cross(p1 - point, p2 - point) / area2;
    float beta = cross(p2 - point, p0 - point) / area2;
    float gamma = 1.0f - alpha - beta;

    return Vec3(alpha, beta, gamma);
}

void Renderer::setPixel(uint32_t x, uint32_t y, const Vec4& color, float depth)
{
    if (x >= m_width || y >= m_height) {
        return;
    }
    uint32_t idx = y * m_width + x;
    if (depth < m_depth_buffer[idx]) {
        m_frame_buffer[idx] = color;
        m_depth_buffer[idx] = depth;
    }
}
