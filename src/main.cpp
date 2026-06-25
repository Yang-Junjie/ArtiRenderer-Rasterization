#include "color.h"
#include "mesh.h"
#include "ppm_writer.h"
#include "renderer.h"

#include <memory>
#include <iostream>
#include <vector>

int main()
{
    Renderer renderer(800, 600);

    std::vector<Vertex> vertices = {
        {Vec3(300.0f, 320.0f, 0.75f), Vec3(0.0f, 0.0f, 0.0f), Vec3(70.0f, 110.0f, 210.0f)},
        {Vec3(500.0f, 320.0f, 0.75f), Vec3(0.0f, 0.0f, 0.0f), Vec3(70.0f, 110.0f, 210.0f)},
        {Vec3(500.0f, 500.0f, 0.75f), Vec3(0.0f, 0.0f, 0.0f), Vec3(70.0f, 110.0f, 210.0f)},
        {Vec3(300.0f, 500.0f, 0.75f), Vec3(0.0f, 0.0f, 0.0f), Vec3(70.0f, 110.0f, 210.0f)},
        {Vec3(400.0f, 160.0f, 0.20f), Vec3(0.0f, 0.0f, 0.0f), Vec3(255.0f, 210.0f, 80.0f)},
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,};

    auto pyramid = std::make_shared<Mesh>(std::move(vertices), std::move(indices));
    renderer.renderMesh(pyramid);

    PPMWriter ppm_writer;
    ppm_writer.setImageSize(800, 600);
    ppm_writer.setFileName("output.ppm");
    ppm_writer.writePPM(renderer.getFrameBuffer());

    std::cout << "Rendered pyramid mesh." << std::endl;
    return 0;
}
