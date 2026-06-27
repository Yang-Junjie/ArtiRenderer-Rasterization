#pragma once
#include "renderer.h"
#include "scene.h"

class SceneRenderer {
public:
    explicit SceneRenderer(Scene& scene);
    ~SceneRenderer() = default;

    void clearBuffers();
    void submitFrameData();
    const std::vector<Vec4>& getFrameBuffer() const;
    void setViewport(uint32_t width, uint32_t height);

private:
    Scene& m_scene;
    Renderer m_renderer;
};
