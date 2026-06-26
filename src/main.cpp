#include "camera.h"
#include "math/vec4.h"
#include "mesh.h"
#include "platform/desktop/glfw_window.h"
#include "renderer.h"

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

    Renderer renderer(kWidth, kHeight);
    renderer.setViewMatrix(camera.getViewMatrix());
    renderer.setProjectionMatrix(camera.getProjectionMatrix());

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

    while (!window.shouldClose()) {
        camera.orbitAroundTarget(0.05f, 0.0f);
        renderer.setViewMatrix(camera.getViewMatrix());

        renderer.clearBuffers();
        renderer.renderMesh(pyramid);
        window.present(renderer.getFrameBuffer());
        window.update();
    }

    return 0;
}
