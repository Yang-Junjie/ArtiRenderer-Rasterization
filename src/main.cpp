#include "camera.h"
#include "entity.h"
#include "model_loader.h"
#include "platform/desktop/glfw_window.h"
#include "scene_renderer.h"
#include "shader.h"

#include <cstdint>
#include <memory>
#include <utility>

namespace {

constexpr float kPi = 3.14159265358979323846f;

std::shared_ptr<Shader> createSceneShader()
{
    return std::make_shared<BlinnPhongShader>(
        Vec3(0.35f, -0.75f, 0.45f), Vec3(1.0f, 0.96f, 0.90f), 0.16f, 0.90f, 0.35f, 48.0f);
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

    Camera camera(Vec3(3.8f, 5.25f, -7.0f), Vec3(0.2f, 0.35f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    camera.setPerspective(60.0f * kPi / 180.0f, static_cast<float>(kWidth) / static_cast<float>(kHeight), 0.1f, 100.0f);

    Scene scene(camera);
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    auto scene_shader = createSceneShader();
    auto rock_model = ModelLoader::get().loadModel("../assets/rock/rock.obj");
    auto teapot_model = ModelLoader::get().loadModel("../assets/teapot/teapot.obj");

    addEntity(scene, rock_model, scene_shader, Vec3(-1.8f, -0.25f, 0.0f), Vec3(0.0f, 0.25f, 0.0f), Vec3(0.75f, 0.75f, 0.75f));
    addEntity(scene, teapot_model, scene_shader, Vec3(1.85f, -0.55f, 0.0f), Vec3(0.0f, -0.35f, 0.0f), Vec3(0.022f, 0.022f, 0.022f));

    while (!window.shouldClose()) {
        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }
    return 0;
}
