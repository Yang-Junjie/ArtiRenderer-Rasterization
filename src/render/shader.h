#pragma once
#include "resource/material.h"
#include "math/mat4.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "resource/mesh.h"

struct ShaderContext {
    Mat4 model_matrix;
    Mat4 view_matrix;
    Mat4 projection_matrix;
    Vec3 camera_position;
    const Material* material = nullptr;
};

struct VertexOut {
    Vec4 clip_space_position;
    Vec3 world_space_position;
    Vec3 normal;
    Vec4 color;
    Vec2 tex_coord;
};

struct FragmentIn {
    Vec3 world_space_position;
    Vec3 normal;
    Vec4 color;
    Vec2 tex_coord;
    Vec2 tex_coord_dx;
    Vec2 tex_coord_dy;
    float depth;
};

struct FragmentOut {
    Vec4 color;
    bool discard = false;
};

class Shader {
public:
    virtual ~Shader() = default;

    virtual VertexOut vertex(const Vertex& in, const ShaderContext& ctx) const = 0;
    virtual FragmentOut fragment(const FragmentIn& in, const ShaderContext& ctx) const = 0;
};

class BlinnPhongShader final : public Shader {
public:
    BlinnPhongShader();
    BlinnPhongShader(const Vec3& light_direction,
                     const Vec3& light_color,
                     float ambient_strength,
                     float diffuse_strength,
                     float specular_strength,
                     float shininess);

    VertexOut vertex(const Vertex& in, const ShaderContext& ctx) const override;
    FragmentOut fragment(const FragmentIn& in, const ShaderContext& ctx) const override;

private:
    Vec3 m_light_direction;
    Vec3 m_light_color;
    float m_ambient_strength;
    float m_diffuse_strength;
    float m_specular_strength;
    float m_shininess;
};

class NormalShader final : public Shader {
public:
    NormalShader() = default;

    VertexOut vertex(const Vertex& in, const ShaderContext& ctx) const override;
    FragmentOut fragment(const FragmentIn& in, const ShaderContext& ctx) const override;
};
