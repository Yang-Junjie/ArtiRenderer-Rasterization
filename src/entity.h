#pragma once
#include "math/mat4.h"
#include "math/vec3.h"
#include "mesh.h"

#include <memory>
#include <utility>

class Entity {
public:
    explicit Entity(std::unique_ptr<Mesh> mesh)
        : m_mesh(std::move(mesh)),
          m_position(Vec3{0.0f, 0.0f, 0.0f}),
          m_rotation(Vec3{0.0f, 0.0f, 0.0f}),
          m_scale(Vec3{1.0f, 1.0f, 1.0f})
    {}

    Mat4 getTransform() const
    {
        return Mat4::translation(m_position) * Mat4::rotationX(m_rotation.x()) * Mat4::rotationY(m_rotation.y()) *
               Mat4::rotationZ(m_rotation.z()) * Mat4::scale(m_scale);
    }

    const Mesh* getMesh() const
    {
        return m_mesh.get();
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
    std::unique_ptr<Mesh> m_mesh;
    Vec3 m_position;
    Vec3 m_rotation;
    Vec3 m_scale;
};
