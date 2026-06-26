#pragma once

#include "vec3.h"

#include <array>
#include <cstddef>
#include <iostream>

class Mat3 {
public:
    using Row = std::array<float, 3>;

    constexpr Mat3() noexcept = default;

    constexpr explicit Mat3(float diagonal) noexcept
        : elements_{Row{diagonal, 0.0f, 0.0f}, Row{0.0f, diagonal, 0.0f}, Row{0.0f, 0.0f, diagonal}}
    {}

    constexpr Mat3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) noexcept
        : elements_{Row{m00, m01, m02}, Row{m10, m11, m12}, Row{m20, m21, m22}}
    {}

    [[nodiscard]] static constexpr Mat3 identity() noexcept
    {
        return Mat3{1.0f};
    }

    [[nodiscard]] constexpr const Row& operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr Row& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Mat3& operator+=(const Mat3& other) noexcept
    {
        for (std::size_t row = 0; row < 3; ++row) {
            for (std::size_t col = 0; col < 3; ++col) {
                elements_[row][col] += other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat3& operator-=(const Mat3& other) noexcept
    {
        for (std::size_t row = 0; row < 3; ++row) {
            for (std::size_t col = 0; col < 3; ++col) {
                elements_[row][col] -= other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat3& operator*=(float scalar) noexcept
    {
        for (auto& row : elements_) {
            for (float& value : row) {
                value *= scalar;
            }
        }
        return *this;
    }

    constexpr Mat3& operator/=(float scalar) noexcept
    {
        return *this *= 1.0f / scalar;
    }

private:
    std::array<Row, 3> elements_{};
};

[[nodiscard]] constexpr Mat3 operator+(Mat3 lhs, const Mat3& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat3 operator-(Mat3 lhs, const Mat3& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat3 operator*(Mat3 lhs, float scalar) noexcept
{
    lhs *= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat3 operator*(float scalar, Mat3 rhs) noexcept
{
    rhs *= scalar;
    return rhs;
}

[[nodiscard]] constexpr Mat3 operator/(Mat3 lhs, float scalar) noexcept
{
    lhs /= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat3 transpose(const Mat3& m) noexcept
{
    return Mat3{
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2],
    };
}

[[nodiscard]] constexpr Mat3 operator*(const Mat3& lhs, const Mat3& rhs) noexcept
{
    Mat3 result{};
    for (std::size_t row = 0; row < 3; ++row) {
        for (std::size_t col = 0; col < 3; ++col) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < 3; ++k) {
                sum += lhs[row][k] * rhs[k][col];
            }
            result[row][col] = sum;
        }
    }
    return result;
}

[[nodiscard]] constexpr Vec3 operator*(const Mat3& lhs, const Vec3& v) noexcept
{
    return Vec3{
        lhs[0][0] * v.x() + lhs[0][1] * v.y() + lhs[0][2] * v.z(),
        lhs[1][0] * v.x() + lhs[1][1] * v.y() + lhs[1][2] * v.z(),
        lhs[2][0] * v.x() + lhs[2][1] * v.y() + lhs[2][2] * v.z(),
    };
}

