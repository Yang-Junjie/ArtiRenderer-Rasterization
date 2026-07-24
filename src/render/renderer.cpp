#include "math/vec4.h"
#include "render/renderer.h"

#include <cmath>
#include <cstdint>

#include <algorithm>

namespace {

template <typename T>
T perspectiveInterpolate(
    const T& v0, const T& v1, const T& v2, float weighted0, float weighted1, float weighted2, float interpolated_w)
{
    return (v0 * weighted0 + v1 * weighted1 + v2 * weighted2) * interpolated_w;
}
} // namespace

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

void Renderer::render(const FrameData& frame_data)
{
    setProjectionMatrix(frame_data.camera.getProjectionMatrix());
    setViewMatrix(frame_data.camera.getViewMatrix());
    for (const auto& mesh_data : frame_data.meshes) {
        renderMesh(
            mesh_data.mesh, mesh_data.material, mesh_data.shader, mesh_data.transform, frame_data.camera.getPosition());
    }
}

void Renderer::renderMesh(const Mesh* mesh,
                          const Material* material,
                          const Shader* shader,
                          const Mat4& transform,
                          const Vec3& camera_position)
{
    if (mesh == nullptr || shader == nullptr) {
        return;
    }

    ShaderContext context;
    context.model_matrix = transform;
    context.view_matrix = m_view_matrix;
    context.projection_matrix = m_proj_matrix;
    context.camera_position = camera_position;
    context.material = material;

    const auto& vertices = mesh->getVertices();
    const auto& indices = mesh->getIndices();

    for (size_t i = 0; i < indices.size(); i += 3) {
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];

        renderTriangle(v0, v1, v2, *shader, context);
    }
}

bool Renderer::projectVertex(const VertexOut& vertex, ProjectedVertex& out_screen) const
{
    const Vec4& clip_pos = vertex.clip_space_position;

    if (clip_pos.w() <= 1e-5f) {
        return false;
    }

    const float inv_w = 1.0f / clip_pos.w();
    const float ndc_x = clip_pos.x() * inv_w;
    const float ndc_y = clip_pos.y() * inv_w;
    const float ndc_z = clip_pos.z() * inv_w;

    const float screen_x = (ndc_x * 0.5f + 0.5f) * static_cast<float>(m_width);
    const float screen_y = (1.0f - (ndc_y * 0.5f + 0.5f)) * static_cast<float>(m_height);

    out_screen = ProjectedVertex{Vec3(screen_x, screen_y, ndc_z), inv_w, vertex};
    return true;
}

void Renderer::renderTriangle(
    const Vertex& v0, const Vertex& v1, const Vertex& v2, const Shader& shader, const ShaderContext& context)
{
    ProjectedVertex t0, t1, t2;
    if (!projectVertex(shader.vertex(v0, context), t0) || !projectVertex(shader.vertex(v1, context), t1) ||
        !projectVertex(shader.vertex(v2, context), t2)) {
        return;
    }

    Vec2 p0(t0.screen_pos.x(), t0.screen_pos.y());
    Vec2 p1(t1.screen_pos.x(), t1.screen_pos.y());
    Vec2 p2(t2.screen_pos.x(), t2.screen_pos.y());

    constexpr float epsilon = 1e-6f;
    const float area2 = cross(p1 - p0, p2 - p0);

    const float inv_area2 = 1.0f / area2;
    const Vec3 barycentric_dx(
        (p1.y() - p2.y()) * inv_area2, (p2.y() - p0.y()) * inv_area2, (p0.y() - p1.y()) * inv_area2);
    const Vec3 barycentric_dy(
        (p2.x() - p1.x()) * inv_area2, (p0.x() - p2.x()) * inv_area2, (p1.x() - p0.x()) * inv_area2);

    const float inv_w_dx =
        t0.inv_w * barycentric_dx.x() + t1.inv_w * barycentric_dx.y() + t2.inv_w * barycentric_dx.z();
    const float inv_w_dy =
        t0.inv_w * barycentric_dy.x() + t1.inv_w * barycentric_dy.y() + t2.inv_w * barycentric_dy.z();

    const VertexOut& o0 = t0.shader_output;
    const VertexOut& o1 = t1.shader_output;
    const VertexOut& o2 = t2.shader_output;

    const Vec2 tex_coord_numerator_dx = o0.tex_coord * (t0.inv_w * barycentric_dx.x()) +
                                        o1.tex_coord * (t1.inv_w * barycentric_dx.y()) +
                                        o2.tex_coord * (t2.inv_w * barycentric_dx.z());
    const Vec2 tex_coord_numerator_dy = o0.tex_coord * (t0.inv_w * barycentric_dy.x()) +
                                        o1.tex_coord * (t1.inv_w * barycentric_dy.y()) +
                                        o2.tex_coord * (t2.inv_w * barycentric_dy.z());

    int minX = static_cast<int>(std::min({t0.screen_pos.x(), t1.screen_pos.x(), t2.screen_pos.x()}));
    int maxX = static_cast<int>(std::max({t0.screen_pos.x(), t1.screen_pos.x(), t2.screen_pos.x()}));
    int minY = static_cast<int>(std::min({t0.screen_pos.y(), t1.screen_pos.y(), t2.screen_pos.y()}));
    int maxY = static_cast<int>(std::max({t0.screen_pos.y(), t1.screen_pos.y(), t2.screen_pos.y()}));

    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, static_cast<int>(m_width) - 1);
    maxY = std::min(maxY, static_cast<int>(m_height) - 1);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Vec2 pixel_pos(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
            Vec3 barycentric = computeBarycentric(pixel_pos, p0, p1, p2);

            const float alpha = barycentric.x();
            const float beta = barycentric.y();
            const float gamma = barycentric.z();

            if (alpha < -epsilon || beta < -epsilon || gamma < -epsilon) {
                continue;
            }

            const float inv_w_alpha = t0.inv_w * alpha;
            const float inv_w_beta = t1.inv_w * beta;
            const float inv_w_gamma = t2.inv_w * gamma;
            const float inv_w_sum = inv_w_alpha + inv_w_beta + inv_w_gamma;
            if (std::abs(inv_w_sum) < epsilon) {
                continue;
            }

            const float interpolated_w = 1.0f / inv_w_sum;
            const float depth = perspectiveInterpolate(t0.screen_pos.z(),
                                                       t1.screen_pos.z(),
                                                       t2.screen_pos.z(),
                                                       inv_w_alpha,
                                                       inv_w_beta,
                                                       inv_w_gamma,
                                                       interpolated_w);

            auto interpolated_world_pos = perspectiveInterpolate(o0.world_space_position,
                                                                 o1.world_space_position,
                                                                 o2.world_space_position,
                                                                 inv_w_alpha,
                                                                 inv_w_beta,
                                                                 inv_w_gamma,
                                                                 interpolated_w);

            auto interpolated_normal = unitVector(perspectiveInterpolate(
                o0.normal, o1.normal, o2.normal, inv_w_alpha, inv_w_beta, inv_w_gamma, interpolated_w));
            auto interpolated_color = perspectiveInterpolate(
                o0.color, o1.color, o2.color, inv_w_alpha, inv_w_beta, inv_w_gamma, interpolated_w);
            auto interpolated_tex_coord = perspectiveInterpolate(
                o0.tex_coord, o1.tex_coord, o2.tex_coord, inv_w_alpha, inv_w_beta, inv_w_gamma, interpolated_w);

            FragmentIn fragment;

            fragment.world_space_position = interpolated_world_pos;
            fragment.normal = interpolated_normal;
            fragment.color = interpolated_color;

            fragment.tex_coord = interpolated_tex_coord;
            fragment.tex_coord_dx = (tex_coord_numerator_dx - fragment.tex_coord * inv_w_dx) * interpolated_w;
            fragment.tex_coord_dy = (tex_coord_numerator_dy - fragment.tex_coord * inv_w_dy) * interpolated_w;
            fragment.depth = depth;

            const FragmentOut out = shader.fragment(fragment, context);
            if (!out.discard) {
                setPixel(static_cast<uint32_t>(x), static_cast<uint32_t>(y), out.color, depth);
            }
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
