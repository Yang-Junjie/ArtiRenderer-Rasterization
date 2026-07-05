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
    frame_data.meshes.reserve(m_scene.getEntities().size());

    for (const auto& entity : m_scene.getEntities()) {
        frame_data.meshes.push_back(
            MeshData{entity.getMesh(), entity.getMaterial(), entity.getShader(), entity.getTransform()});
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
