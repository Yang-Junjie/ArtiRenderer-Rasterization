#pragma once
#include "color.h"
#include "math/vec2.h"
#include "mesh.h"

#include <memory>
#include <vector>

class Renderer {
public:
    Renderer(uint32_t width, uint32_t height);
    ~Renderer() = default;

    void clearBuffers();
    void renderTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    void renderMesh(const std::shared_ptr<Mesh>& mesh);

    const std::vector<Color>& getFrameBuffer() const
    {
        return m_frame_buffer;
    }

    void resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        m_frame_buffer.resize(width * height, Vec3(0.0, 0.0, 0.0));
        m_depth_buffer.resize(width * height, std::numeric_limits<float>::max());
    }

private:
    void setPixel(uint32_t x, uint32_t y, const Color& color, float depth = 0.0f);
    Vec3 computeBarycentric(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2);
    // bool isInsideTriangle(const Vec2& point, const Vec2& p0, const Vec2& p1, const Vec2& p2);

private:
    uint32_t m_width;
    uint32_t m_height;
    std::vector<float> m_depth_buffer;
    std::vector<Color> m_frame_buffer;
};
