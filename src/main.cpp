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

    Camera camera(Vec3(3.0f, 5.4f, -5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(
        60.0f * 3.14159f / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    const std::vector<Vertex> vertices = {
        {Vec3(-2.0f, 0.0f, -1.5f), Vec3(0.0f, 1.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(0.0f, 1.0f, 0.0f)},
        {Vec3(-2.0f, 0.0f, 1.5f), Vec3(0.0f, 1.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(0.0f, 0.0f, 0.0f)},
        {Vec3(2.0f, 0.0f, 1.5f), Vec3(0.0f, 1.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(1.0f, 0.0f, 0.0f)},
        {Vec3(2.0f, 0.0f, -1.5f), Vec3(0.0f, 1.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(1.0f, 1.0f, 0.0f)},
    };
    const std::vector<uint32_t> indices = {
        0,
        1,
        2,
        0,
        2,
        3,
    };

    auto material = std::make_unique<Material>();
    bool texture_loaded = false;

    material->albedo.loadFromFIle("../assets/brownie_cake.png");
    material->albedo.setAddressMode(AddressMode::Clamp);

    Entity textured_quad(std::make_unique<Mesh>(vertices, indices), std::move(material));
    scene.addEntity(std::move(textured_quad));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }

    return 0;
}
