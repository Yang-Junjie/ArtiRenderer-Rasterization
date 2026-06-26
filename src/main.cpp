#include "camera.h"
#include "color.h"
#include "mesh.h"
#include "ppm_writer.h"
#include "renderer.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    constexpr uint32_t kWidth = 800;
    constexpr uint32_t kHeight = 600;

    Camera camera(Vec3(0.0f, 3.0f, 8.0f),   
                  Vec3(0.0f, 1.0f, 0.0f),   
                  Vec3(0.0f, 1.0f, 0.0f));  
    camera.setPerspective(60.0f * 3.14159f / 180.0f,   
                          static_cast<float>(kWidth) / static_cast<float>(kHeight),
                          0.1f,   
                          100.0f); 

    Renderer renderer(kWidth, kHeight);
    renderer.setViewMatrix(camera.getViewMatrix());
    renderer.setProjectionMatrix(camera.getProjectionMatrix());


    std::vector<Vertex> vertices = {
        {Vec3(-1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(255.0f, 80.0f, 80.0f)},  
        {Vec3( 1.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(80.0f, 255.0f, 80.0f)},   
        {Vec3( 1.0f, 0.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(80.0f, 80.0f, 255.0f)},   
        {Vec3(-1.0f, 0.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(255.0f, 255.0f, 80.0f)},
        {Vec3( 0.0f, 2.0f,  0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(255.0f, 255.0f, 255.0f)}, 
    };
    std::vector<uint32_t> indices = {
        0, 1, 2,  
        0, 2, 3,   
        0, 1, 4,   
        1, 2, 4,   
        2, 3, 4,   
        3, 0, 4,   
    };

    auto pyramid = std::make_shared<Mesh>(std::move(vertices), std::move(indices));
    renderer.renderMesh(pyramid);

    PPMWriter ppmWriter;
    ppmWriter.setImageSize(kWidth, kHeight);
    ppmWriter.setFileName("output.ppm");
    ppmWriter.writePPM(renderer.getFrameBuffer());

    return 0;
}
