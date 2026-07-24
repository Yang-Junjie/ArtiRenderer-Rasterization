#include "resource/mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices))
{}

const std::vector<Vertex>& Mesh::getVertices() const
{
    return m_vertices;
}

const std::vector<uint32_t>& Mesh::getIndices() const
{
    return m_indices;
}
