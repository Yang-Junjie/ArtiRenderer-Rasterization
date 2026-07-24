#pragma once
#include "math/mat4.h"
#include "math/vec3.h"
#include "render/shader.h"
#include "resource/model.h"

#include <memory>
#include <utility>

class Entity {
public:
    Entity(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
        : m_model(std::move(model)),
          m_shader(std::move(shader)),
          m_position(Vec3{0.0f, 0.0f, 0.0f}),
          m_rotation(Vec3{0.0f, 0.0f, 0.0f}),
          m_scale(Vec3{1.0f, 1.0f, 1.0f})
    {}

    Mat4 getTransform() const
    {
        return Mat4::translation(m_position) * Mat4::rotationX(m_rotation.x()) * Mat4::rotationY(m_rotation.y()) *
               Mat4::rotationZ(m_rotation.z()) * Mat4::scale(m_scale);
    }

    const std::shared_ptr<Model>& getModel() const
    {
        return m_model;
    }

    const Shader* getShader() const
    {
        return m_shader.get();
    }

    void setModel(std::shared_ptr<Model> model)
    {
        m_model = std::move(model);
    }

    void setShader(std::shared_ptr<Shader> shader)
    {
        m_shader = std::move(shader);
    }

    void setPosition(const Vec3& position)
    {
        m_position = position;
    }

    void setRotation(const Vec3& rotation)
    {
        m_rotation = rotation;
    }

    void setScale(const Vec3& scale)
    {
        m_scale = scale;
    }

private:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<Shader> m_shader;
    Vec3 m_position;
    Vec3 m_rotation;
    Vec3 m_scale;
};
