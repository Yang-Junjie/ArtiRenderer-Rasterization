#include "camera.h"
#include "entity.h"
#include "mesh.h"
#include "platform/desktop/glfw_window.h"
#include "ppm_writer.h"
#include "scene_renderer.h"
#include "texture.h"

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
    SceneRenderer scene_renderer(scene);
    scene_renderer.setViewport(kWidth, kHeight);

    const std::vector<Vertex> vertices = {
        {Vec3(-1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 80.0f, 80.0f, 255.0f)},
        {Vec3(1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(80.0f, 255.0f, 80.0f, 255.0f)},
        {Vec3(1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(80.0f, 80.0f, 255.0f, 255.0f)},
        {Vec3(-1.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 255.0f, 80.0f, 255.0f)},
        {Vec3(0.0f, 2.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec4(255.0f, 255.0f, 255.0f, 255.0f)},
    };
    const std::vector<uint32_t> indices = {
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

    auto makePyramidMesh = [&]() {
        return std::make_unique<Mesh>(vertices, indices);
    };

    Entity pyramid_center(makePyramidMesh());
    pyramid_center.setPosition(Vec3(0.0f, 0.0f, 0.0f));

    Entity pyramid_left(makePyramidMesh());
    pyramid_left.setPosition(Vec3(-3.0f, 0.0f, 0.0f));
    pyramid_left.setRotation(Vec3(0.0f, 0.6f, 0.0f));

    Entity pyramid_right(makePyramidMesh());
    pyramid_right.setPosition(Vec3(3.0f, 0.0f, 0.0f));
    pyramid_right.setScale(Vec3(0.75f, 0.75f, 0.75f));

    scene.addEntity(std::move(pyramid_center));
    scene.addEntity(std::move(pyramid_left));
    scene.addEntity(std::move(pyramid_right));

    while (!window.shouldClose()) {
        scene.getCamera().orbitAroundTarget(0.05f, 0.0f);

        scene_renderer.clearBuffers();
        scene_renderer.submitFrameData();
        window.present(scene_renderer.getFrameBuffer());
        window.update();
    }

    // Texture texture;
    // texture.loadFromFIle("../assets/brownie_cake.png");
    // std::cout << "Texture loaded: " << texture.getWidth() << "x" << texture.getHeight() << " " <<
    // texture.getChannels()
    //           << " channels" << std::endl;

    // const int width = texture.getWidth();
    // const int height = texture.getHeight();
    // const int channels = texture.getChannels();
    // const std::vector<uint8_t> pixels = texture.getPixels();

    // Renderer renderer(width, height);
    // for (int y = 0; y < height; ++y) {
    //     for (int x = 0; x < width; ++x) {
    //         const int src = (y * width + x) * channels;
    //         const uint8_t r = pixels[src + 0];
    //         const uint8_t g = channels > 1 ? pixels[src + 1] : r;
    //         const uint8_t b = channels > 2 ? pixels[src + 2] : r;
    //         renderer.setPixel(x, y, Vec4(r, g, b, 255.0f));
    //     }
    // }

    // PPMWriter ppmWriter;
    // ppmWriter.setFileName("brownie_cake.ppm");
    // ppmWriter.setImageSize(width, height);
    // ppmWriter.writePPM(renderer.getFrameBuffer());

    return 0;
}
