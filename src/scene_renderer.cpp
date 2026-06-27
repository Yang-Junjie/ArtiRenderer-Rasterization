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
    FrameData frameData{m_scene.getCamera(), {}};
    frameData.meshes.reserve(m_scene.getMeshes().size());

    for (const auto& mesh : m_scene.getMeshes()) {
        frameData.meshes.push_back(MeshData{mesh, Mat4::identity()});
    }

    m_renderer.render(frameData);
}

const std::vector<Vec4>& SceneRenderer::getFrameBuffer() const
{
    return m_renderer.getFrameBuffer();
}

void SceneRenderer::setViewport(uint32_t width, uint32_t height)
{
    m_renderer.setViewport(width, height);
}
