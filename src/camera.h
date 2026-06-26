#pragma once
#include "math/mat4.h"
#include "math/vec3.h"

#include <cstdint>

class Camera {
public:
    Camera(const Vec3& position, const Vec3& target, const Vec3& up);

    void setPerspective(float fov, float aspect, float near, float far);
    
    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix() const;
    Mat4 getViewProjectionMatrix() const;

    void setPosition(const Vec3& position);
    void setTarget(const Vec3& target);
    void setUp(const Vec3& up);

    Vec3 getPosition() const;
    Vec3 getTarget() const;
    Vec3 getUp() const;

    float getFov() const;
    float getAspectRatio() const;
    float getNear() const;
    float getFar() const;

private:
    Vec3 m_position;
    Vec3 m_target;
    Vec3 m_up;

    // radians
    float m_fov;
    float m_aspectRatio;
    float m_near;
    float m_far;

    // radians
    float m_pitch;

    // radians
    float m_yaw;

    uint32_t m_viewportWidth;
    uint32_t m_viewportHeight;
};
