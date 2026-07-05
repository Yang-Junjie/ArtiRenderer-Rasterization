#include "camera.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "platform/desktop/glfw_window.h"
#include "scene_renderer.h"
#include "shader.h"

#include <cmath>
#include <cstdint>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr float kPi = 3.14159265358979323846f;

std::unique_ptr<Shader> createSceneShader()
{
    return std::make_unique<BlinnPhongShader>(
        Vec3(0.35f, -0.75f, 0.45f), Vec3(1.0f, 0.96f, 0.90f), 0.16f, 0.90f, 0.35f, 48.0f);
}

std::unique_ptr<Mesh>
    createSphereMesh(float radius, uint32_t latitude_segments, uint32_t longitude_segments, const Vec4& color)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(static_cast<size_t>(latitude_segments + 1) * static_cast<size_t>(longitude_segments + 1));
    indices.reserve(static_cast<size_t>(latitude_segments) * static_cast<size_t>(longitude_segments) * 6);

    for (uint32_t lat = 0; lat <= latitude_segments; ++lat) {
        const float v = static_cast<float>(lat) / static_cast<float>(latitude_segments);
        const float phi = v * kPi;
        const float sin_phi = std::sin(phi);
        const float cos_phi = std::cos(phi);

        for (uint32_t lon = 0; lon <= longitude_segments; ++lon) {
            const float u = static_cast<float>(lon) / static_cast<float>(longitude_segments);
            const float theta = u * 2.0f * kPi;
            const float sin_theta = std::sin(theta);
            const float cos_theta = std::cos(theta);

            const Vec3 normal(sin_phi * cos_theta, cos_phi, sin_phi * sin_theta);
            vertices.push_back(Vertex{normal * radius, normal, color, Vec2(u, v)});
        }
    }

    const uint32_t stride = longitude_segments + 1;
    for (uint32_t lat = 0; lat < latitude_segments; ++lat) {
        for (uint32_t lon = 0; lon < longitude_segments; ++lon) {
            const uint32_t i0 = lat * stride + lon;
            const uint32_t i1 = i0 + 1;
            const uint32_t i2 = i0 + stride;
            const uint32_t i3 = i2 + 1;

            if (lat != 0) {
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);
            }

            if (lat != latitude_segments - 1) {
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }
    }

    return std::make_unique<Mesh>(std::move(vertices), std::move(indices));
}

std::unique_ptr<Mesh> createCubeMesh(float size, const Vec4& color, float texture_repeats, bool flip_v = false)
{
    const float half_size = size * 0.5f;
    const float min_v = flip_v ? texture_repeats : 0.0f;
    const float max_v = flip_v ? 0.0f : texture_repeats;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(24);
    indices.reserve(36);

    const auto add_face = [&](const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& normal) {
        const uint32_t base = static_cast<uint32_t>(vertices.size());
        vertices.push_back(Vertex{p0, normal, color, Vec2(0.0f, min_v)});
        vertices.push_back(Vertex{p1, normal, color, Vec2(texture_repeats, min_v)});
        vertices.push_back(Vertex{p2, normal, color, Vec2(texture_repeats, max_v)});
        vertices.push_back(Vertex{p3, normal, color, Vec2(0.0f, max_v)});

        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    };

    add_face(Vec3(-half_size, -half_size, -half_size),
             Vec3(half_size, -half_size, -half_size),
             Vec3(half_size, half_size, -half_size),
             Vec3(-half_size, half_size, -half_size),
             Vec3(0.0f, 0.0f, -1.0f));
    add_face(Vec3(half_size, -half_size, half_size),
             Vec3(-half_size, -half_size, half_size),
             Vec3(-half_size, half_size, half_size),
             Vec3(half_size, half_size, half_size),
             Vec3(0.0f, 0.0f, 1.0f));
    add_face(Vec3(-half_size, -half_size, half_size),
             Vec3(-half_size, -half_size, -half_size),
             Vec3(-half_size, half_size, -half_size),
             Vec3(-half_size, half_size, half_size),
             Vec3(-1.0f, 0.0f, 0.0f));
    add_face(Vec3(half_size, -half_size, -half_size),
             Vec3(half_size, -half_size, half_size),
             Vec3(half_size, half_size, half_size),
             Vec3(half_size, half_size, -half_size),
             Vec3(1.0f, 0.0f, 0.0f));
    add_face(Vec3(-half_size, half_size, -half_size),
             Vec3(half_size, half_size, -half_size),
             Vec3(half_size, half_size, half_size),
             Vec3(-half_size, half_size, half_size),
             Vec3(0.0f, 1.0f, 0.0f));
    add_face(Vec3(-half_size, -half_size, half_size),
             Vec3(half_size, -half_size, half_size),
             Vec3(half_size, -half_size, -half_size),
             Vec3(-half_size, -half_size, -half_size),
             Vec3(0.0f, -1.0f, 0.0f));

    return std::make_unique<Mesh>(std::move(vertices), std::move(indices));
}

std::unique_ptr<Material> createCopperMaterial()
{
    auto material = std::make_unique<Material>();
    material->ambient_strength = 0.08f;
    material->diffuse_strength = 0.70f;
    material->specular_strength = 0.85f;
    material->shininess = 128.0f;
    material->metallic = 0.45f;
    material->specular_color = Vec3(0.96f, 0.64f, 0.48f);
    return material;
}

std::unique_ptr<Material> createPlasterMaterial()
{
    auto material = std::make_unique<Material>();
    std::string texture_path = "../assets/brownie_cake.png";

    if (material->albedo.loadFromFIle(texture_path)) {
        material->albedo.setAddressMode(AddressMode::Clamp);
        material->ambient_strength = 0.24f;
        material->diffuse_strength = 0.95f;
        material->specular_strength = 0.06f;
        material->shininess = 12.0f;
        material->metallic = 0.0f;
        material->specular_color = Vec3(0.45f, 0.45f, 0.45f);
        return material;
    }
    return nullptr;
}

std::unique_ptr<Material> createBrickMaterial()
{
    auto material = std::make_unique<Material>();
    std::string texture_path = "../assets/wall.jpg";

    if (material->albedo.loadFromFIle(texture_path)) {
        material->albedo.setAddressMode(AddressMode::Repeat);
        material->ambient_strength = 0.15f;
        material->diffuse_strength = 0.90f;
        material->specular_strength = 0.04f;
        material->shininess = 10.0f;
        material->metallic = 0.0f;
        material->specular_color = Vec3(0.35f, 0.32f, 0.28f);
        return material;
    }
    return nullptr;
}

} // namespace

int main()
{
    constexpr uint32_t kWidth = 1'920;
    constexpr uint32_t kHeight = 1'080;

    GlfwWindow window(kWidth, kHeight, "Soft Rasterization");

    Camera camera(Vec3(0.0f, 1.35f, -6.4f), Vec3(0.0f, 0.15f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(60.0f * kPi / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    Entity copper_sphere(createSphereMesh(0.9f, 48, 96, Vec4(184.0f, 104.0f, 55.0f, 255.0f)),
                         createCopperMaterial(),
                         createSceneShader());
    copper_sphere.setPosition(Vec3(-2.0f, 0.0f, 0.0f));
    scene.addEntity(std::move(copper_sphere));

    Entity plaster_cube(createCubeMesh(1.45f, Vec4(222.0f, 216.0f, 202.0f, 255.0f), 1.0f, true),
                        createPlasterMaterial(),
                        createSceneShader());
    plaster_cube.setPosition(Vec3(0.0f, 0.0f, 0.0f));
    plaster_cube.setRotation(Vec3(0.0f, 0.45f, 0.0f));
    scene.addEntity(std::move(plaster_cube));

    Entity brick_cube(
        createCubeMesh(1.45f, Vec4(255.0f, 255.0f, 255.0f, 255.0f), 2.0f), createBrickMaterial(), createSceneShader());
    brick_cube.setPosition(Vec3(2.0f, 0.0f, 0.0f));
    brick_cube.setRotation(Vec3(0.0f, -0.45f, 0.0f));
    scene.addEntity(std::move(brick_cube));

    Entity normal_sphere(createSphereMesh(0.65f, 32, 64, Vec4(255.0f, 255.0f, 255.0f, 255.0f)),
                         nullptr,
                         std::make_unique<NormalShader>());
    normal_sphere.setPosition(Vec3(3.75f, 0.0f, 0.0f));
    scene.addEntity(std::move(normal_sphere));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }
    return 0;
}
