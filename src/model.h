#pragma once
#include "material.h"
#include "mesh.h"

#include <memory>
#include <utility>
#include <vector>

struct SubMesh {
    SubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material = nullptr)
        : m_mesh(std::move(mesh)),
          m_material(std::move(material))
    {}

    void setMesh(std::unique_ptr<Mesh> mesh)
    {
        m_mesh = std::move(mesh);
    }

    void setMaterial(std::unique_ptr<Material> material)
    {
        m_material = std::move(material);
    }

    const Mesh* getMesh() const
    {
        return m_mesh.get();
    }

    const Material* getMaterial() const
    {
        return m_material.get();
    }

private:
    std::unique_ptr<Mesh> m_mesh;
    std::unique_ptr<Material> m_material;
};

class Model {
public:
    Model() = default;

    void addSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material = nullptr)
    {
        m_sub_meshes.emplace_back(std::move(mesh), std::move(material));
    }

    const std::vector<SubMesh>& getSubMeshes() const
    {
        return m_sub_meshes;
    }

private:
    std::vector<SubMesh> m_sub_meshes;
};
