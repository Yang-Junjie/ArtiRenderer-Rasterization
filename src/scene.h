#pragma once
#include "camera.h"
#include "mesh.h"
#include "renderer.h"

#include <memory>
#include <vector>

class Scene {
public:
    Scene(const Camera& camera, const Renderer& renderer)
        : m_camera(camera),
          m_renderer(renderer)
    {}

    void addMesh(const std::shared_ptr<Mesh>& mesh)
    {
        m_meshes.push_back(mesh);
    }

    void render()
    {
        m_renderer.clearBuffers();
        for (auto& mesh : m_meshes) {
            m_renderer.renderMesh(mesh);
        }
    }

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    Camera m_camera;
    Renderer m_renderer;
};
