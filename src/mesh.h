#pragma once
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

#include <cstdint>

#include <vector>

struct Vertex {
    // position z component range:[0,1]
    Vec3 position;
    Vec3 normal;
    Vec4 color;
    Vec2 texCoord;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    ~Mesh() = default;

    const std::vector<Vertex>& getVertices() const;
    const std::vector<uint32_t>& getIndices() const;

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
};
