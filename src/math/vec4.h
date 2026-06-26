#pragma once

#include "vec3.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>

class Vec4 {
public:
    constexpr Vec4() noexcept = default;

    constexpr Vec4(float x, float y, float z, float w) noexcept
        : elements_{x, y, z, w}
    {}

    constexpr Vec4(const Vec3& v, float w = 1.0f) noexcept
        : elements_{v.x(), v.y(), v.z(), w}
    {}

    [[nodiscard]] constexpr float x() const noexcept
    {
        return elements_[0];
    }

    [[nodiscard]] constexpr float y() const noexcept
    {
        return elements_[1];
    }

    [[nodiscard]] constexpr float z() const noexcept
    {
        return elements_[2];
    }

    [[nodiscard]] constexpr float w() const noexcept
    {
        return elements_[3];
    }

    [[nodiscard]] constexpr Vec4 operator-() const noexcept
    {
        return Vec4{-elements_[0], -elements_[1], -elements_[2], -elements_[3]};
    }

    [[nodiscard]] constexpr float operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr float& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Vec4& operator+=(const Vec4& other) noexcept
    {
        elements_[0] += other.elements_[0];
        elements_[1] += other.elements_[1];
        elements_[2] += other.elements_[2];
        elements_[3] += other.elements_[3];
        return *this;
    }

    constexpr Vec4& operator*=(float scalar) noexcept
    {
        elements_[0] *= scalar;
        elements_[1] *= scalar;
        elements_[2] *= scalar;
        elements_[3] *= scalar;
        return *this;
    }

    constexpr Vec4& operator/=(float scalar) noexcept
    {
        return *this *= 1.0f / scalar;
    }

    [[nodiscard]] float length() const noexcept
    {
        return std::sqrt(lengthSquared());
    }

    [[nodiscard]] constexpr float lengthSquared() const noexcept
    {
        return elements_[0] * elements_[0] + elements_[1] * elements_[1] +
               elements_[2] * elements_[2] + elements_[3] * elements_[3];
    }

    [[nodiscard]] bool nearZero() const noexcept
    {
        constexpr auto threshold = 1e-8f;
        return std::abs(elements_[0]) < threshold && std::abs(elements_[1]) < threshold &&
               std::abs(elements_[2]) < threshold && std::abs(elements_[3]) < threshold;
    }

private:
    std::array<float, 4> elements_{};
};

inline std::ostream& operator<<(std::ostream& out, const Vec4& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z() << ' ' << v.w();
}

[[nodiscard]] constexpr Vec4 operator+(const Vec4& lhs, const Vec4& rhs) noexcept
{
    return Vec4{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z(), lhs.w() + rhs.w()};
}

[[nodiscard]] constexpr Vec4 operator-(const Vec4& lhs, const Vec4& rhs) noexcept
{
    return Vec4{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z(), lhs.w() - rhs.w()};
}

[[nodiscard]] constexpr Vec4 operator*(const Vec4& lhs, const Vec4& rhs) noexcept
{
    return Vec4{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z(), lhs.w() * rhs.w()};
}

[[nodiscard]] constexpr Vec4 operator*(float scalar, const Vec4& v) noexcept
{
    return Vec4{scalar * v.x(), scalar * v.y(), scalar * v.z(), scalar * v.w()};
}

[[nodiscard]] constexpr Vec4 operator*(const Vec4& v, float scalar) noexcept
{
    return scalar * v;
}

[[nodiscard]] constexpr Vec4 operator/(const Vec4& v, float scalar) noexcept
{
    return (1.0f / scalar) * v;
}

[[nodiscard]] constexpr float dot(const Vec4& lhs, const Vec4& rhs) noexcept
{
    return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z() + lhs.w() * rhs.w();
}

[[nodiscard]] inline Vec4 unitVector(const Vec4& v)
{
    return v / v.length();
}

using vec4 = Vec4;
