#pragma once

#include "camera.h"
#include "math/mat4.h"
#include "mesh.h"

#include <vector>

struct MeshData {
    const Mesh* mesh = nullptr;
    Mat4 transform{Mat4::identity()};
};

struct FrameData {
    Camera camera;
    std::vector<MeshData> meshes;
};
