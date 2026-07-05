#pragma once

#include "camera.h"
#include "material.h"
#include "math/mat4.h"
#include "mesh.h"
#include "shader.h"

#include <vector>

struct MeshData {
    const Mesh* mesh = nullptr;
    const Material* material = nullptr;
    const Shader* shader = nullptr;
    Mat4 transform{Mat4::identity()};
};

struct FrameData {
    Camera camera;
    std::vector<MeshData> meshes;
};
