#pragma once

#include "camera.h"
#include "mesh.h"

#include <memory>
#include <vector>

struct MeshData {
    std::shared_ptr<Mesh> mesh;
    Mat4 transform;
};

struct FrameData {
    Camera camera;
    std::vector<MeshData> meshes;
};
