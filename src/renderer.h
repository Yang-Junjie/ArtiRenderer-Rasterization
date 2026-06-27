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

    void render(const FrameData& frameData);

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

private:
    void setPixel(uint32_t x, uint32_t y, const Vec4& color, float depth = 0.0f);
    Vec3 computeBarycentric(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2);
    void renderTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    void renderMesh(const Mesh* mesh, const Mat4& transform);

    // Transform a world-space position through MVP, perspective divide, and viewport.
    // Returns screen-space (x, y, ndcZ) via outScreen.
    // Returns false if the vertex is behind the near plane and should be culled.
    bool transformVertex(const Vec3& worldPos, Vec3& outScreen) const;

private:
    uint32_t m_width;
    uint32_t m_height;
    std::vector<float> m_depth_buffer;
    std::vector<Vec4> m_frame_buffer;

    Mat4 m_model_matrix{Mat4::identity()};
    Mat4 m_view_matrix{Mat4::identity()};
    Mat4 m_proj_matrix{Mat4::identity()};
};
