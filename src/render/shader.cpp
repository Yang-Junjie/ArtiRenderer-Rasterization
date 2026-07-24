#include "math/vec2.h"
#include "math/vec3.h"
#include "render/shader.h"

#include <cmath>

#include <algorithm>

BlinnPhongShader::BlinnPhongShader()
    : BlinnPhongShader(Vec3(0.35f, -0.75f, 0.45f), Vec3(1.0f, 0.96f, 0.90f), 0.16f, 0.90f, 0.35f, 48.0f)
{}

BlinnPhongShader::BlinnPhongShader(const Vec3& light_direction,
                                   const Vec3& light_color,
                                   float ambient_strength,
                                   float diffuse_strength,
                                   float specular_strength,
                                   float shininess)
    : m_light_direction(unitVector(light_direction)),
      m_light_color(light_color),
      m_ambient_strength(ambient_strength),
      m_diffuse_strength(diffuse_strength),
      m_specular_strength(specular_strength),
      m_shininess(shininess)
{}

VertexOut BlinnPhongShader::vertex(const Vertex& in, const ShaderContext& ctx) const
{
    const Vec4 world_position = ctx.model_matrix * Vec4(in.position, 1.0f);
    const Vec4 world_normal = ctx.model_matrix * Vec4(in.normal, 0.0f);

    VertexOut out;
    out.clip_space_position = ctx.projection_matrix * ctx.view_matrix * world_position;
    out.world_space_position = Vec3(world_position.x(), world_position.y(), world_position.z());
    out.normal = unitVector(Vec3(world_normal.x(), world_normal.y(), world_normal.z()));
    out.color = in.color;
    out.tex_coord = in.texCoord;
    return out;
}

FragmentOut BlinnPhongShader::fragment(const FragmentIn& in, const ShaderContext& ctx) const
{
    Vec4 base_color = in.color;
    float ambient_strength = m_ambient_strength;
    float diffuse_strength = m_diffuse_strength;
    float specular_strength = m_specular_strength;
    float shininess = m_shininess;
    float metallic = 0.0f;
    Vec3 specular_color(1.0f, 1.0f, 1.0f);

    if (ctx.material != nullptr) {
        if (ctx.material->albedo.getWidth() > 0 && ctx.material->albedo.getHeight() > 0) {
            base_color = ctx.material->albedo.sampleAnisotropic(
                in.tex_coord.x(), in.tex_coord.y(), in.tex_coord_dx, in.tex_coord_dy);
        }

        ambient_strength = ctx.material->ambient_strength;
        diffuse_strength = ctx.material->diffuse_strength;
        specular_strength = ctx.material->specular_strength;
        shininess = ctx.material->shininess;
        metallic = std::clamp(ctx.material->metallic, 0.0f, 1.0f);
        specular_color = ctx.material->specular_color;
    }

    const Vec3 albedo = Vec3(base_color.x(), base_color.y(), base_color.z()) / 255.0f;
    const Vec3 normal = unitVector(in.normal);
    const Vec3 light_dir = unitVector(-m_light_direction);
    const Vec3 view_dir = unitVector(ctx.camera_position - in.world_space_position);
    const Vec3 half_dir = unitVector(light_dir + view_dir);

    const float diffuse_term = std::max(dot(normal, light_dir), 0.0f);
    const float specular_term = std::pow(std::max(dot(normal, half_dir), 0.0f), shininess);

    const Vec3 specular_tint = specular_color * (1.0f - metallic) + albedo * metallic;
    const Vec3 ambient = albedo * ambient_strength;
    const Vec3 diffuse = albedo * m_light_color * (diffuse_strength * diffuse_term * (1.0f - metallic));
    const Vec3 specular = specular_tint * m_light_color * (specular_strength * specular_term);
    const Vec3 lit_color = ambient + diffuse + specular;

    FragmentOut out{Vec4(lit_color.x() * 255.0f, lit_color.y() * 255.0f, lit_color.z() * 255.0f, base_color.w()),
                    false};

    return out;
}

VertexOut NormalShader::vertex(const Vertex& in, const ShaderContext& ctx) const
{
    const Vec4 world_position = ctx.model_matrix * Vec4(in.position, 1.0f);
    const Vec4 world_normal = ctx.model_matrix * Vec4(in.normal, 0.0f);

    VertexOut out;
    out.clip_space_position = ctx.projection_matrix * ctx.view_matrix * world_position;
    out.world_space_position = Vec3(world_position.x(), world_position.y(), world_position.z());
    out.normal = unitVector(Vec3(world_normal.x(), world_normal.y(), world_normal.z()));
    out.color = in.color;
    out.tex_coord = in.texCoord;
    return out;
}

FragmentOut NormalShader::fragment(const FragmentIn& in, const ShaderContext& ctx) const
{
    const Vec3 normal = unitVector(in.normal);
    const Vec3 color = Vec3(normal.x() * 0.5f + 0.5f, normal.y() * 0.5f + 0.5f, normal.z() * 0.5f + 0.5f);

    return FragmentOut{Vec4(color.x(), color.y(), color.z(), 1.0f) * 255.0f, false};
}
