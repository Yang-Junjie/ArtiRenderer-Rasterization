#include "camera.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "platform/desktop/glfw_window.h"
#include "scene_renderer.h"

#include <cstdint>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

int main()
{
    constexpr uint32_t kWidth = 1'920;
    constexpr uint32_t kHeight = 1'080;

    GlfwWindow window(kWidth, kHeight, "Soft Rasterization");

    Camera camera(Vec3(4.0f, 1.7f, -7.0f), Vec3(0.0f, 0.0f, 34.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(
        60.0f * 3.14159f / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    constexpr float kGroundHalfWidth = 8.0f;
    constexpr float kGroundNearZ = 0.0f;
    constexpr float kGroundFarZ = 80.0f;
    constexpr float kTextureRepeatsAcross = 10.0f;
    constexpr float kTextureRepeatsForward = 10.0f;

    const std::vector<Vertex> vertices = {
        {Vec3(-kGroundHalfWidth, 0.0f, kGroundNearZ),
         Vec3(0.0f, 1.0f, 0.0f),
         Vec4(255.0f, 255.0f, 255.0f, 255.0f),
         Vec2(0.0f, 0.0f)},
        {Vec3(-kGroundHalfWidth, 0.0f, kGroundFarZ),
         Vec3(0.0f, 1.0f, 0.0f),
         Vec4(255.0f, 255.0f, 255.0f, 255.0f),
         Vec2(0.0f, kTextureRepeatsForward)},
        {Vec3(kGroundHalfWidth, 0.0f, kGroundFarZ),
         Vec3(0.0f, 1.0f, 0.0f),
         Vec4(255.0f, 255.0f, 255.0f, 255.0f),
         Vec2(kTextureRepeatsAcross, kTextureRepeatsForward)},
        {Vec3(kGroundHalfWidth, 0.0f, kGroundNearZ),
         Vec3(0.0f, 1.0f, 0.0f),
         Vec4(255.0f, 255.0f, 255.0f, 255.0f),
         Vec2(kTextureRepeatsAcross, 0.0f)},
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

    constexpr int kStripeTextureSize = 64;
    constexpr int kStripeChannels = 3;
    constexpr int kStripeHalfWidth = 2;

    material->albedo = Texture(kStripeTextureSize, kStripeTextureSize, kStripeChannels);
    auto& pixels = material->albedo.getPixels();
    const int stripe_center = kStripeTextureSize / 2;
    for (int y = 0; y < kStripeTextureSize; ++y) {
        for (int x = 0; x < kStripeTextureSize; ++x) {
            const bool in_vertical_stripe =
                x >= stripe_center - kStripeHalfWidth && x <= stripe_center + kStripeHalfWidth;
            const bool in_horizontal_stripe =
                y >= stripe_center - kStripeHalfWidth && y <= stripe_center + kStripeHalfWidth;
            const uint8_t value = (in_vertical_stripe || in_horizontal_stripe) ? 0 : 255;
            const int index = (y * kStripeTextureSize + x) * kStripeChannels;

            pixels[index] = value;
            pixels[index + 1] = value;
            pixels[index + 2] = value;
        }
    }
    material->albedo.setAddressMode(AddressMode::Repeat);

    Entity textured_quad(std::make_unique<Mesh>(vertices, indices), std::move(material));
    scene.addEntity(std::move(textured_quad));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }

    // constexpr uint32_t kWidth = 1280;
    // constexpr uint32_t kHeight = 720;

    // GlfwWindow window(kWidth, kHeight, "Mipmap Generation Test");

    // Camera camera(Vec3(0.0f, 0.0f, 8.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    // camera.setPerspective(
    //     45.0f * 3.14159f / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    // Scene scene(camera);
    // SceneRenderer scene_renderer(scene);
    // scene_renderer.setViewport(kWidth, kHeight);

    // Texture mip_source;
    // if (!mip_source.loadFromFIle("../assets/brownie_cake.png")) {
    //     return 1;
    // }

    // const auto mipSizeAtLevel = [](int base_size, int level) {
    //     int size = base_size;
    //     for (int i = 0; i < level; ++i) {
    //         size = std::max(1, (size + 1) / 2);
    //     }
    //     return size;
    // };

    // const auto makeQuad = [](float center_x, float center_y, float size) {
    //     const float half_size = size * 0.5f;
    //     const float left = center_x - half_size;
    //     const float right = center_x + half_size;
    //     const float bottom = center_y - half_size;
    //     const float top = center_y + half_size;

    // std::vector<Vertex> vertices = {
    //     {Vec3(left, bottom, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(0.0f, 1.0f,
    //     0.0f)}, {Vec3(left, top, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f), Vec3(0.0f,
    //     0.0f, 0.0f)}, {Vec3(right, top, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f),
    //     Vec3(1.0f, 0.0f, 0.0f)}, {Vec3(right, bottom, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec4(255.0f, 255.0f, 255.0f,
    //     255.0f), Vec3(1.0f, 1.0f, 0.0f)},
    // };

    // const std::vector<uint32_t> indices = {
    //     0,
    //     1,
    //     2,
    //     0,
    //     2,
    //     3,
    // };

    // return std::make_unique<Mesh>(std::move(vertices), indices);
    // };

    // const Texture& mip_source_view = mip_source;
    // const int mip_level_count = mip_source.getMipLevelCount();
    // const int base_width = mip_source.getWidth();
    // const int base_height = mip_source.getHeight();
    // const int texture_channels = mip_source.getChannels();
    // constexpr float kQuadSize = 1.0f;
    // constexpr float kQuadGap = 0.12f;
    // const float row_width =
    //     static_cast<float>(mip_level_count) * kQuadSize + static_cast<float>(mip_level_count - 1) * kQuadGap;
    // const float start_x = row_width * -0.5f + kQuadSize * 0.5f;

    // for (int level = 0; level < mip_level_count; ++level) {
    //     const int mip_width = mipSizeAtLevel(base_width, level);
    //     const int mip_height = mipSizeAtLevel(base_height, level);

    // auto material = std::make_unique<Material>();
    // material->albedo = Texture(mip_width, mip_height, texture_channels);
    // material->albedo.setAddressMode(AddressMode::Clamp);
    // material->albedo.getPixels() = mip_source_view.getPixels(level);

    // const float center_x = start_x + static_cast<float>(level) * (kQuadSize + kQuadGap);
    // Entity mip_quad(makeQuad(center_x, 0.0f, kQuadSize), std::move(material));
    // scene.addEntity(std::move(mip_quad));
    // }

    // while (!window.shouldClose()) {
    //     scene_renderer.clearBuffers();
    //     scene_renderer.submitFrameData();
    //     window.present(scene_renderer.getFrameBuffer());
    //     window.update();
    // }

    return 0;
}
