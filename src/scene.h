#pragma once
#include "camera.h"
#include "mesh.h"

#include <memory>
#include <vector>

class Scene {
public:
    Scene(const Camera& camera)
        : m_camera(camera)
    {}

    void addMesh(const std::shared_ptr<Mesh>& mesh)
    {
        m_meshes.push_back(mesh);
    }

    std::vector<std::shared_ptr<Mesh>>& getMeshes()
    {
        return m_meshes;
    }

    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const
    {
        return m_meshes;
    }

    Camera& getCamera()
    {
        return m_camera;
    }

    const Camera& getCamera() const
    {
        return m_camera;
    }

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    Camera m_camera;
};
