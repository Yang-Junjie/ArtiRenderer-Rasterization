#pragma once

#include <cmath>
#include <cstddef>

#include <array>
#include <iostream>

class Vec2 {
public:
    constexpr Vec2() noexcept = default;

    constexpr Vec2(float x, float y) noexcept
        : elements_{x, y}
    {}

    [[nodiscard]] constexpr float x() const noexcept
    {
        return elements_[0];
    }

    [[nodiscard]] constexpr float y() const noexcept
    {
        return elements_[1];
    }

    [[nodiscard]] constexpr Vec2 operator-() const noexcept
    {
        return Vec2{-elements_[0], -elements_[1]};
    }

    [[nodiscard]] constexpr float operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr float& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Vec2& operator+=(const Vec2& other) noexcept
    {
        elements_[0] += other.elements_[0];
        elements_[1] += other.elements_[1];
        return *this;
    }

    constexpr Vec2& operator*=(float scalar) noexcept
    {
        elements_[0] *= scalar;
        elements_[1] *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(float scalar) noexcept
    {
        return *this *= 1.0 / scalar;
    }

    [[nodiscard]] float length() const noexcept
    {
        return std::sqrt(lengthSquared());
    }

    [[nodiscard]] constexpr float lengthSquared() const noexcept
    {
        return elements_[0] * elements_[0] + elements_[1] * elements_[1];
    }

    [[nodiscard]] bool nearZero() const noexcept
    {
        constexpr auto threshold = 1e-8;
        return std::abs(elements_[0]) < threshold && std::abs(elements_[1]) < threshold;
    }

private:
    std::array<float, 2> elements_{};
};

inline std::ostream& operator<<(std::ostream& out, const Vec2& v)
{
    return out << v.x() << ' ' << v.y();
}

[[nodiscard]] constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs) noexcept
{
    return Vec2{lhs.x() + rhs.x(), lhs.y() + rhs.y()};
}

[[nodiscard]] constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs) noexcept
{
    return Vec2{lhs.x() - rhs.x(), lhs.y() - rhs.y()};
}

[[nodiscard]] constexpr Vec2 operator*(const Vec2& lhs, const Vec2& rhs) noexcept
{
    return Vec2{lhs.x() * rhs.x(), lhs.y() * rhs.y()};
}

[[nodiscard]] constexpr Vec2 operator*(float scalar, const Vec2& v) noexcept
{
    return Vec2{scalar * v.x(), scalar * v.y()};
}

[[nodiscard]] constexpr Vec2 operator*(const Vec2& v, float scalar) noexcept
{
    return scalar * v;
}

[[nodiscard]] constexpr Vec2 operator/(const Vec2& v, float scalar) noexcept
{
    return (1.0 / scalar) * v;
}

[[nodiscard]] constexpr float dot(const Vec2& lhs, const Vec2& rhs) noexcept
{
    return lhs.x() * rhs.x() + lhs.y() * rhs.y();
}

[[nodiscard]] inline Vec2 unitVector(const Vec2& v)
{
    return v / v.length();
}

[[nodiscard]] inline float cross(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.x() * rhs.y() - lhs.y() * rhs.x();
}
