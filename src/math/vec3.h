#pragma once

#include <cmath>
#include <cstddef>

#include <array>
#include <iostream>

class Vec3 {
public:
    constexpr Vec3() noexcept = default;

    constexpr Vec3(float x, float y, float z) noexcept
        : elements_{x, y, z}
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

    [[nodiscard]] constexpr Vec3 operator-() const noexcept
    {
        return Vec3{-elements_[0], -elements_[1], -elements_[2]};
    }

    [[nodiscard]] constexpr float operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr float& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Vec3& operator+=(const Vec3& other) noexcept
    {
        elements_[0] += other.elements_[0];
        elements_[1] += other.elements_[1];
        elements_[2] += other.elements_[2];
        return *this;
    }

    constexpr Vec3& operator*=(float scalar) noexcept
    {
        elements_[0] *= scalar;
        elements_[1] *= scalar;
        elements_[2] *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(float scalar) noexcept
    {
        return *this *= 1.0 / scalar;
    }

    [[nodiscard]] float length() const noexcept
    {
        return std::sqrt(lengthSquared());
    }

    [[nodiscard]] constexpr float lengthSquared() const noexcept
    {
        return elements_[0] * elements_[0] + elements_[1] * elements_[1] + elements_[2] * elements_[2];
    }

    [[nodiscard]] bool nearZero() const noexcept
    {
        constexpr auto threshold = 1e-8;
        return std::abs(elements_[0]) < threshold && std::abs(elements_[1]) < threshold &&
               std::abs(elements_[2]) < threshold;
    }

private:
    std::array<float, 3> elements_{};
};

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

[[nodiscard]] constexpr Vec3 operator+(const Vec3& lhs, const Vec3& rhs) noexcept
{
    return Vec3{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

[[nodiscard]] constexpr Vec3 operator-(const Vec3& lhs, const Vec3& rhs) noexcept
{
    return Vec3{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

[[nodiscard]] constexpr Vec3 operator*(const Vec3& lhs, const Vec3& rhs) noexcept
{
    return Vec3{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z()};
}

[[nodiscard]] constexpr Vec3 operator*(float scalar, const Vec3& v) noexcept
{
    return Vec3{scalar * v.x(), scalar * v.y(), scalar * v.z()};
}

[[nodiscard]] constexpr Vec3 operator*(const Vec3& v, float scalar) noexcept
{
    return scalar * v;
}

[[nodiscard]] constexpr Vec3 operator/(const Vec3& v, float scalar) noexcept
{
    return (1.0 / scalar) * v;
}

[[nodiscard]] constexpr float dot(const Vec3& lhs, const Vec3& rhs) noexcept
{
    return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

[[nodiscard]] constexpr Vec3 cross(const Vec3& lhs, const Vec3& rhs) noexcept
{
    return Vec3{
        lhs.y() * rhs.z() - lhs.z() * rhs.y(),
        lhs.z() * rhs.x() - lhs.x() * rhs.z(),
        lhs.x() * rhs.y() - lhs.y() * rhs.x(),
    };
}

[[nodiscard]] inline Vec3 unitVector(const Vec3& v)
{
    return v / v.length();
}
