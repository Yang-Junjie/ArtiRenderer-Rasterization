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

    void setPitch(float pitch);
    void setYaw(float yaw);
    void orbitAroundTarget(float yaw_delta, float pitch_delta);

private:
    void updatePositionFromOrbit();
    void updateOrbitFromPosition();

private:
    Vec3 m_position;
    Vec3 m_target;
    Vec3 m_up;

    // radian
    float m_fov;
    float m_aspect_ratio;
    float m_near;
    float m_far;

    // radian
    float m_pitch;

    // radian
    float m_yaw;

    float m_target_distance;

    uint32_t m_viewport_width;
    uint32_t m_viewport_height;
};
