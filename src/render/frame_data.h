#pragma once

#include "render/shader.h"
#include "resource/material.h"
#include "resource/mesh.h"
#include "scene/camera.h"
#include "math/mat4.h"

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
