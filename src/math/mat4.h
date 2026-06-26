#pragma once

#include "vec3.h"
#include "vec4.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>

class Mat4 {
public:
    using Row = std::array<float, 4>;

    constexpr Mat4() noexcept = default;

    constexpr explicit Mat4(float diagonal) noexcept
        : elements_{Row{diagonal, 0.0f, 0.0f, 0.0f},
                    Row{0.0f, diagonal, 0.0f, 0.0f},
                    Row{0.0f, 0.0f, diagonal, 0.0f},
                    Row{0.0f, 0.0f, 0.0f, diagonal}}
    {}

    constexpr Mat4(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33) noexcept
        : elements_{Row{m00, m01, m02, m03},
                    Row{m10, m11, m12, m13},
                    Row{m20, m21, m22, m23},
                    Row{m30, m31, m32, m33}}
    {}

    [[nodiscard]] static constexpr Mat4 identity() noexcept
    {
        return Mat4{1.0f};
    }

    [[nodiscard]] static constexpr Mat4 translation(float x, float y, float z) noexcept
    {
        return Mat4{
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    [[nodiscard]] static constexpr Mat4 translation(const Vec3& t) noexcept
    {
        return translation(t.x(), t.y(), t.z());
    }

    [[nodiscard]] static constexpr Mat4 scale(float x, float y, float z) noexcept
    {
        return Mat4{
            x,     0.0f, 0.0f, 0.0f,
            0.0f,  y,     0.0f, 0.0f,
            0.0f, 0.0f,   z,    0.0f,
            0.0f, 0.0f,   0.0f, 1.0f,
        };
    }

    [[nodiscard]] static constexpr Mat4 scale(const Vec3& s) noexcept
    {
        return scale(s.x(), s.y(), s.z());
    }

    [[nodiscard]] static Mat4 rotationX(float radians) noexcept
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return Mat4{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c,    -s,    0.0f,
            0.0f, s,     c,    0.0f,
            0.0f, 0.0f,  0.0f, 1.0f,
        };
    }

    [[nodiscard]] static Mat4 rotationY(float radians) noexcept
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return Mat4{
            c,    0.0f, s,    0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -s,   0.0f, c,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    [[nodiscard]] static Mat4 rotationZ(float radians) noexcept
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return Mat4{
            c,   -s,   0.0f, 0.0f,
            s,    c,   0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }

    [[nodiscard]] constexpr const Row& operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr Row& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Mat4& operator+=(const Mat4& other) noexcept
    {
        for (std::size_t row = 0; row < 4; ++row) {
            for (std::size_t col = 0; col < 4; ++col) {
                elements_[row][col] += other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat4& operator-=(const Mat4& other) noexcept
    {
        for (std::size_t row = 0; row < 4; ++row) {
            for (std::size_t col = 0; col < 4; ++col) {
                elements_[row][col] -= other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat4& operator*=(float scalar) noexcept
    {
        for (auto& row : elements_) {
            for (float& value : row) {
                value *= scalar;
            }
        }
        return *this;
    }

    constexpr Mat4& operator/=(float scalar) noexcept
    {
        return *this *= 1.0f / scalar;
    }

private:
    std::array<Row, 4> elements_{};
};

[[nodiscard]] constexpr Mat4 operator+(Mat4 lhs, const Mat4& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat4 operator-(Mat4 lhs, const Mat4& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat4 operator*(Mat4 lhs, float scalar) noexcept
{
    lhs *= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat4 operator*(float scalar, Mat4 rhs) noexcept
{
    rhs *= scalar;
    return rhs;
}

[[nodiscard]] constexpr Mat4 operator/(Mat4 lhs, float scalar) noexcept
{
    lhs /= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat4 transpose(const Mat4& m) noexcept
{
    return Mat4{
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3],
    };
}

[[nodiscard]] constexpr Mat4 operator*(const Mat4& lhs, const Mat4& rhs) noexcept
{
    Mat4 result{};
    for (std::size_t row = 0; row < 4; ++row) {
        for (std::size_t col = 0; col < 4; ++col) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < 4; ++k) {
                sum += lhs[row][k] * rhs[k][col];
            }
            result[row][col] = sum;
        }
    }
    return result;
}

[[nodiscard]] constexpr Vec4 operator*(const Mat4& lhs, const Vec4& v) noexcept
{
    return Vec4{
        lhs[0][0] * v.x() + lhs[0][1] * v.y() + lhs[0][2] * v.z() + lhs[0][3] * v.w(),
        lhs[1][0] * v.x() + lhs[1][1] * v.y() + lhs[1][2] * v.z() + lhs[1][3] * v.w(),
        lhs[2][0] * v.x() + lhs[2][1] * v.y() + lhs[2][2] * v.z() + lhs[2][3] * v.w(),
        lhs[3][0] * v.x() + lhs[3][1] * v.y() + lhs[3][2] * v.z() + lhs[3][3] * v.w(),
    };
}

