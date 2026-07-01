#include "camera.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "platform/desktop/glfw_window.h"
#include "scene_renderer.h"

#include <array>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

int main()
{
    constexpr uint32_t kWidth = 800;
    constexpr uint32_t kHeight = 600;

    GlfwWindow window(kWidth, kHeight, "Soft Rasterization");

    Camera camera(Vec3(3.0f, 2.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(
        60.0f * 3.14159f / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    const Vec4 white(255.0f, 255.0f, 255.0f, 255.0f);
    const std::vector<Vertex> vertices = {
        // Front face
        {Vec3(-1.0f, -1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, -1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(-1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), white, Vec3(0.0f, 0.0f, 0.0f)},

        // Back face
        {Vec3(1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(-1.0f, 1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, -1.0f), Vec3(0.0f, 0.0f, -1.0f), white, Vec3(0.0f, 0.0f, 0.0f)},

        // Left face
        {Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1.0f, 0.0f, 0.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(-1.0f, -1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(-1.0f, 1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(-1.0f, 1.0f, -1.0f), Vec3(-1.0f, 0.0f, 0.0f), white, Vec3(0.0f, 0.0f, 0.0f)},

        // Right face
        {Vec3(1.0f, -1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, -1.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), white, Vec3(0.0f, 0.0f, 0.0f)},

        // Top face
        {Vec3(-1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, 1.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(-1.0f, 1.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), white, Vec3(0.0f, 0.0f, 0.0f)},

        // Bottom face
        {Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), white, Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, -1.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), white, Vec3(1.0f, 1.0f, 0.0f)},
        {Vec3(1.0f, -1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), white, Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(-1.0f, -1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), white, Vec3(0.0f, 0.0f, 0.0f)},
    };
    const std::vector<uint32_t> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };

    auto material = std::make_unique<Material>();
    bool texture_loaded = false;

    material->albedo.loadFromFIle("../assets/brownie_cake.png");

    Entity textured_cube(std::make_unique<Mesh>(vertices, indices), std::move(material));
    scene.addEntity(std::move(textured_cube));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }

    return 0;
}
