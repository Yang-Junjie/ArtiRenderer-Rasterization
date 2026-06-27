#include "camera.h"
#include "scene_renderer.h"
#include "platform/desktop/glfw_window.h"

#include <memory>
#include <utility>
#include <vector>

int main()
{
    constexpr uint32_t kWidth = 800;
    constexpr uint32_t kHeight = 600;

    GlfwWindow window(kWidth, kHeight, "Soft Rasterization");

    Camera camera(Vec3(0.0f, 3.0f, 8.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(
        60.0f * 3.14159f / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer sceneRenderer(scene);
    sceneRenderer.setViewport(kWidth, kHeight);

    std::vector<Vertex> vertices = {
        {Vec3(-1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 80.0f, 80.0f, 255.0f)},
        {Vec3(1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(80.0f, 255.0f, 80.0f, 255.0f)},
        {Vec3(1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(80.0f, 80.0f, 255.0f, 255.0f)},
        {Vec3(-1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 255.0f, 80.0f, 255.0f)},
        {Vec3(0.0f, 2.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f)},
    };
    std::vector<uint32_t> indices = {
        0,
        1,
        2,
        0,
        2,
        3,
        0,
        1,
        4,
        1,
        2,
        4,
        2,
        3,
        4,
        3,
        0,
        4,
    };

    auto pyramid = std::make_shared<Mesh>(std::move(vertices), std::move(indices));
    scene.addMesh(pyramid);

    while (!window.shouldClose()) {
        scene.getCamera().orbitAroundTarget(0.05f, 0.0f);

        sceneRenderer.clearBuffers();
        sceneRenderer.submitFrameData();
        window.present(sceneRenderer.getFrameBuffer());
        window.update();
    }

    return 0;
}
