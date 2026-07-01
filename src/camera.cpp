#include "camera.h"

#include <cmath>

#include <algorithm>

Camera::Camera(const Vec3& position, const Vec3& target, const Vec3& up)
    : m_position(position),
      m_target(target),
      m_up(up),
      m_fov(45.0f * 3.14159f / 180.0f),
      m_aspect_ratio(1.0f),
      m_near(0.1f),
      m_far(100.0f),
      m_pitch(0.0f),
      m_yaw(0.0f),
      m_target_distance(1.0f),
      m_viewport_width(800),
      m_viewport_height(600)
{
    updateOrbitFromPosition();
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
    m_fov = fov;
    m_aspect_ratio = aspect;
    m_near = near;
    m_far = far;
}

Mat4 Camera::getViewMatrix() const
{
    Vec3 forward = unitVector(m_target - m_position);
    Vec3 right = unitVector(cross(forward, m_up));
    Vec3 up = unitVector(cross(right, forward));

    Mat4 view;
    view[0][0] = right.x();
    view[0][1] = right.y();
    view[0][2] = right.z();
    view[0][3] = -dot(m_position, right);

    view[1][0] = up.x();
    view[1][1] = up.y();
    view[1][2] = up.z();
    view[1][3] = -dot(m_position, up);

    view[2][0] = -forward.x();
    view[2][1] = -forward.y();
    view[2][2] = -forward.z();
    view[2][3] = dot(m_position, forward);

    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;
    view[3][3] = 1.0f;

    return view;
}

Mat4 Camera::getProjectionMatrix() const
{
    float tan_half_fov = std::tan(m_fov * 0.5f);
    float near = m_near;
    float far = m_far;
    float aspect = m_aspect_ratio;

    Mat4 proj;
    proj[0][0] = 1.0f / (aspect * tan_half_fov);
    proj[1][1] = 1.0f / tan_half_fov;
    proj[2][2] = -(far + near) / (far - near);
    proj[2][3] = -(2.0f * far * near) / (far - near);
    proj[3][2] = -1.0f;
    proj[3][3] = 0.0f;

    return proj;
}

Mat4 Camera::getViewProjectionMatrix() const
{

    return getProjectionMatrix() * getViewMatrix();
}

void Camera::setPosition(const Vec3& position)
{
    m_position = position;
    updateOrbitFromPosition();
}

void Camera::setTarget(const Vec3& target)
{
    m_target = target;
    updateOrbitFromPosition();
}

void Camera::setUp(const Vec3& up)
{
    m_up = up;
}

Vec3 Camera::getPosition() const
{
    return m_position;
}

Vec3 Camera::getTarget() const
{
    return m_target;
}

Vec3 Camera::getUp() const
{
    return m_up;
}

float Camera::getFov() const
{
    return m_fov;
}

float Camera::getAspectRatio() const
{
    return m_aspect_ratio;
}

float Camera::getNear() const
{
    return m_near;
}

float Camera::getFar() const
{
    return m_far;
}

void Camera::setPitch(float pitch)
{
    constexpr float kPitchLimit = 1.5f;
    m_pitch = std::clamp(pitch, -kPitchLimit, kPitchLimit);
    updatePositionFromOrbit();
}

void Camera::setYaw(float yaw)
{
    m_yaw = yaw;
    updatePositionFromOrbit();
}

void Camera::orbitAroundTarget(float yaw_delta, float pitch_delta)
{
    m_yaw += yaw_delta;
    setPitch(m_pitch + pitch_delta);
}

void Camera::updatePositionFromOrbit()
{
    const float cosPitch = std::cos(m_pitch);

    Vec3 offset(m_target_distance * cosPitch * std::sin(m_yaw),
                m_target_distance * std::sin(m_pitch),
                m_target_distance * cosPitch * std::cos(m_yaw));

    m_position = m_target + offset;
}

void Camera::updateOrbitFromPosition()
{
    Vec3 offset = m_position - m_target;
    m_target_distance = offset.length();

    if (m_target_distance <= 0.0001f) {
        m_target_distance = 0.0001f;
        m_pitch = 0.0f;
        m_yaw = 0.0f;
        return;
    }

    m_pitch = std::asin(offset.y() / m_target_distance);
    m_yaw = std::atan2(offset.x(), offset.z());
}
