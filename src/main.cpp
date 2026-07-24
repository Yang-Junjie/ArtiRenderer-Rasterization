#include "scene/camera.h"
#include "scene/entity.h"
#include "platform/desktop/glfw_window.h"
#include "render/scene_renderer.h"
#include "render/shader.h"
#include "resource/model.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

constexpr float kPi = 3.14159265358979323846f;

std::shared_ptr<Shader> createSceneShader()
{
    return std::make_shared<BlinnPhongShader>(
        Vec3(0.35f, -0.75f, 0.45f), Vec3(1.0f, 0.96f, 0.90f), 0.22f, 0.78f, 0.12f, 32.0f);
}

std::unique_ptr<Material> createTexturedMaterial(const char* texture_path)
{
    auto material = std::make_unique<Material>();
    if (!material->albedo.loadFromFIle(texture_path)) {
        throw std::runtime_error("Failed to load texture: " + std::string(texture_path));
    }
    material->albedo.generateMipmaps();
    material->ambient_strength = 0.22f;
    material->diffuse_strength = 0.78f;
    material->specular_strength = 0.12f;
    material->shininess = 32.0f;
    return material;
}

std::shared_ptr<Model> createPanel(float width, float height, const char* texture_path)
{
    constexpr Vec4 kWhite(255.0f, 255.0f, 255.0f, 255.0f);
    const float half_width = width * 0.5f;
    const float half_height = height * 0.5f;
    const Vec3 normal(0.0f, 0.0f, -1.0f);

    std::vector<Vertex> vertices{
        {{-half_width, -half_height, 0.0f}, normal, kWhite, {0.0f, 1.0f}},
        {{half_width, -half_height, 0.0f}, normal, kWhite, {1.0f, 1.0f}},
        {{half_width, half_height, 0.0f}, normal, kWhite, {1.0f, 0.0f}},
        {{-half_width, half_height, 0.0f}, normal, kWhite, {0.0f, 0.0f}},
    };
    std::vector<uint32_t> indices{0, 1, 2, 0, 2, 3};

    auto model = std::make_shared<Model>();
    model->addSubMesh(std::make_unique<Mesh>(std::move(vertices), std::move(indices)),
                      createTexturedMaterial(texture_path));
    return model;
}

std::shared_ptr<Model> createSphere(float radius, uint32_t slices, uint32_t stacks, const char* texture_path)
{
    constexpr Vec4 kWhite(255.0f, 255.0f, 255.0f, 255.0f);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve((slices + 1) * (stacks + 1));
    indices.reserve(slices * stacks * 6);

    for (uint32_t stack = 0; stack <= stacks; ++stack) {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float theta = v * kPi;
        const float sin_theta = std::sin(theta);
        const float cos_theta = std::cos(theta);

        for (uint32_t slice = 0; slice <= slices; ++slice) {
            const float u = static_cast<float>(slice) / static_cast<float>(slices);
            const float phi = u * 2.0f * kPi;
            const Vec3 normal(sin_theta * std::cos(phi), cos_theta, sin_theta * std::sin(phi));
            vertices.push_back({normal * radius, normal, kWhite, Vec2(1.0f - u, v)});
        }
    }

    for (uint32_t stack = 0; stack < stacks; ++stack) {
        for (uint32_t slice = 0; slice < slices; ++slice) {
            const uint32_t top_left = stack * (slices + 1) + slice;
            const uint32_t top_right = top_left + 1;
            const uint32_t bottom_left = top_left + slices + 1;
            const uint32_t bottom_right = bottom_left + 1;
            indices.insert(indices.end(), {top_left, bottom_left, bottom_right, top_left, bottom_right, top_right});
        }
    }

    auto model = std::make_shared<Model>();
    model->addSubMesh(std::make_unique<Mesh>(std::move(vertices), std::move(indices)),
                      createTexturedMaterial(texture_path));
    return model;
}

void addEntity(Scene& scene,
               std::shared_ptr<Model> model,
               std::shared_ptr<Shader> shader,
               const Vec3& position,
               const Vec3& rotation = Vec3(0.0f, 0.0f, 0.0f),
               const Vec3& scale = Vec3(1.0f, 1.0f, 1.0f))
{
    Entity entity(std::move(model), std::move(shader));
    entity.setPosition(position);
    entity.setRotation(rotation);
    entity.setScale(scale);
    scene.addEntity(std::move(entity));
}

} // namespace

int main()
{
    constexpr uint32_t kWidth = 1'920;
    constexpr uint32_t kHeight = 1'080;

    GlfwWindow window(kWidth, kHeight, "Soft Rasterization");

    Camera camera(Vec3(0.0f, 2.3f, -7.5f), Vec3(0.0f, 0.1f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(60.0f * kPi / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    auto scene_shader = createSceneShader();
    auto panel_model = createPanel(2.8f, 3.9f, "../assets/brownie_cake.png");
    auto sphere_model = createSphere(1.35f, 32, 16, "../assets/wall.jpg");

    addEntity(scene, panel_model, scene_shader, Vec3(-1.75f, 0.0f, 0.0f));
    addEntity(scene, sphere_model, scene_shader, Vec3(1.75f, -0.35f, 0.0f));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }
    return 0;
}
