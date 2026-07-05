#pragma once
#include "math/vec3.h"
#include "texture.h"

class Material {
public:
    Texture albedo;
    Vec3 specular_color{1.0f, 1.0f, 1.0f};
    float ambient_strength = 0.16f;
    float diffuse_strength = 0.90f;
    float specular_strength = 0.35f;
    float shininess = 48.0f;
    float metallic = 0.0f;
};
