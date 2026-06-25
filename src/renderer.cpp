#include "renderer.h"

#include <cstdint>

#include <memory>

Renderer::Renderer(uint32_t width, uint32_t height)
    : m_width(width),
      m_height(height)
{
    m_frame_buffer.resize(width * height, Vec3(0.0, 0.0, 0.0));
    m_depth_buffer.resize(width * height, std::numeric_limits<float>::max());
}

void Renderer::clearBuffers()
{
    std::fill(m_frame_buffer.begin(), m_frame_buffer.end(), Vec3(0.0, 0.0, 0.0));
    std::fill(m_depth_buffer.begin(), m_depth_buffer.end(), std::numeric_limits<float>::max());
}

void Renderer::renderMesh(const std::shared_ptr<Mesh>& mesh)
{
    auto& vertices = mesh->getVertices();
    auto& indices = mesh->getIndices();
    for (size_t i = 0; i < indices.size(); i += 3) {
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];

        renderTriangle(v0, v1, v2);
    }
}

void Renderer::renderTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{

    Vec2 p0(v0.position.x(), v0.position.y());
    Vec2 p1(v1.position.x(), v1.position.y());
    Vec2 p2(v2.position.x(), v2.position.y());

    uint32_t minX = static_cast<uint32_t>(std::min({v0.position.x(), v1.position.x(), v2.position.x()}));
    uint32_t maxX = static_cast<uint32_t>(std::max({v0.position.x(), v1.position.x(), v2.position.x()}));
    uint32_t minY = static_cast<uint32_t>(std::min({v0.position.y(), v1.position.y(), v2.position.y()}));
    uint32_t maxY = static_cast<uint32_t>(std::max({v0.position.y(), v1.position.y(), v2.position.y()}));

    for (uint32_t y = minY; y <= maxY; ++y) {
        for (uint32_t x = minX; x <= maxX; ++x) {
            Vec2 pixel_pos(x + 0.5f, y + 0.5f);
            Vec3 barycentric = computeBarycentric(pixel_pos, p0, p1, p2);

            const float alpha = barycentric.x();
            const float beta = barycentric.y();
            const float gamma = barycentric.z();

            constexpr float epsilon = 1e-6f;
            if (alpha < -epsilon || beta < -epsilon || gamma < -epsilon) {
                continue;
            }
            float depth = v0.position.z() * alpha + v1.position.z() * beta + v2.position.z() * gamma;
            Vec3 color = v0.color * alpha + v1.color * beta + v2.color * gamma;

            setPixel(x, y, color, depth);
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

// bool Renderer::isInsideTriangle(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2)
// {
//     Vec2 edge0 = p1 - p0;
//     Vec2 edge1 = p2 - p1;
//     Vec2 edge2 = p0 - p2;

// bool b1 = cross(edge0, point - p0) >= 0.0;
// bool b2 = cross(edge1, point - p1) >= 0.0;
// bool b3 = cross(edge2, point - p2) >= 0.0;

// return (b1 && b2 && b3) || (!b1 && !b2 && !b3);
// }

void Renderer::setPixel(uint32_t x, uint32_t y, const Color& color, float depth)
{
    if (x >= m_width || y >= m_height) {
        return;
    }
    int idx = y * m_width + x;
    if (depth < m_depth_buffer[idx]) {
        m_frame_buffer[idx] = color;
        m_depth_buffer[idx] = depth;
    }
}
