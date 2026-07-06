#include "frame_data.h"
#include "scene_renderer.h"

SceneRenderer::SceneRenderer(Scene& scene)
    : m_scene(scene)
{}

void SceneRenderer::clearBuffers()
{
    m_renderer.clearBuffers();
}

void SceneRenderer::submitFrameData()
{
    FrameData frame_data{m_scene.getCamera(), {}};
    size_t mesh_count = 0;
    for (const auto& entity : m_scene.getEntities()) {
        mesh_count += entity.getModel()->getSubMeshes().size();
    }
    frame_data.meshes.reserve(mesh_count);

    for (const auto& entity : m_scene.getEntities()) {
        const auto model = entity.getModel();
        for (const auto& sub_mesh : model->getSubMeshes()) {
            frame_data.meshes.push_back(
                MeshData{sub_mesh.getMesh(), sub_mesh.getMaterial(), entity.getShader(), entity.getTransform()});
        }
    }

    m_renderer.render(frame_data);
}

const std::vector<Vec4>& SceneRenderer::getFrameBuffer() const
{
    return m_renderer.getFrameBuffer();
}

void SceneRenderer::setViewport(uint32_t width, uint32_t height)
{
    m_renderer.setViewport(width, height);
}
