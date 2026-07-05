#pragma once
#include "frame_data.h"
#include "math/mat4.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "mesh.h"

#include <limits>
#include <vector>

class Renderer {
public:
    Renderer();
    Renderer(uint32_t width, uint32_t height);
    ~Renderer() = default;

    void clearBuffers();

    void render(const FrameData& frame_data);

    void setViewMatrix(const Mat4& view)
    {
        m_view_matrix = view;
    }

    void setProjectionMatrix(const Mat4& proj)
    {
        m_proj_matrix = proj;
    }

    const std::vector<Vec4>& getFrameBuffer() const
    {
        return m_frame_buffer;
    }

    void setViewport(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        m_frame_buffer.resize(width * height, Vec4(0.0, 0.0, 0.0, 1.0));
        m_depth_buffer.resize(width * height, std::numeric_limits<float>::max());
    }

    void setPixel(uint32_t x, uint32_t y, const Vec4& color, float depth = 0.0f);

private:
    struct ProjectedVertex {
        Vec3 screen_pos;
        float inv_w;
        VertexOut shader_output;
    };

    Vec3 computeBarycentric(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2);
    void renderTriangle(const Vertex& v0,
                        const Vertex& v1,
                        const Vertex& v2,
                        const Shader& shader,
                        const ShaderContext& context);
    void renderMesh(const Mesh* mesh,
                    const Material* material,
                    const Shader* shader,
                    const Mat4& transform,
                    const Vec3& camera_position);

    // Project a clip-space shader output through perspective divide and viewport.
    // Returns screen-space (x, y, ndcZ) via outScreen.
    // Returns false if the vertex is behind the near plane and should be culled.
    bool projectVertex(const VertexOut& vertex, ProjectedVertex& out_screen) const;

private:
    uint32_t m_width;
    uint32_t m_height;
    std::vector<float> m_depth_buffer;
    std::vector<Vec4> m_frame_buffer;

    Mat4 m_view_matrix{Mat4::identity()};
    Mat4 m_proj_matrix{Mat4::identity()};
};
