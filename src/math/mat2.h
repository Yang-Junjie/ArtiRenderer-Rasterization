#pragma once

#include "vec2.h"

#include <array>
#include <cstddef>


class Mat2 {
public:
    using Row = std::array<float, 2>;

    constexpr Mat2() noexcept = default;

    constexpr explicit Mat2(float diagonal) noexcept
        : elements_{Row{diagonal, 0.0f}, Row{0.0f, diagonal}}
    {}

    constexpr Mat2(float m00, float m01, float m10, float m11) noexcept
        : elements_{Row{m00, m01}, Row{m10, m11}}
    {}

    [[nodiscard]] static constexpr Mat2 identity() noexcept
    {
        return Mat2{1.0f};
    }

    [[nodiscard]] constexpr const Row& operator[](std::size_t index) const noexcept
    {
        return elements_[index];
    }

    constexpr Row& operator[](std::size_t index) noexcept
    {
        return elements_[index];
    }

    constexpr Mat2& operator+=(const Mat2& other) noexcept
    {
        for (std::size_t row = 0; row < 2; ++row) {
            for (std::size_t col = 0; col < 2; ++col) {
                elements_[row][col] += other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat2& operator-=(const Mat2& other) noexcept
    {
        for (std::size_t row = 0; row < 2; ++row) {
            for (std::size_t col = 0; col < 2; ++col) {
                elements_[row][col] -= other[row][col];
            }
        }
        return *this;
    }

    constexpr Mat2& operator*=(float scalar) noexcept
    {
        for (auto& row : elements_) {
            for (float& value : row) {
                value *= scalar;
            }
        }
        return *this;
    }

    constexpr Mat2& operator/=(float scalar) noexcept
    {
        return *this *= 1.0f / scalar;
    }

private:
    std::array<Row, 2> elements_{};
};

[[nodiscard]] constexpr Mat2 operator+(Mat2 lhs, const Mat2& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat2 operator-(Mat2 lhs, const Mat2& rhs) noexcept
{
    lhs -= rhs;
    return lhs;
}

[[nodiscard]] constexpr Mat2 operator*(Mat2 lhs, float scalar) noexcept
{
    lhs *= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat2 operator*(float scalar, Mat2 rhs) noexcept
{
    rhs *= scalar;
    return rhs;
}

[[nodiscard]] constexpr Mat2 operator/(Mat2 lhs, float scalar) noexcept
{
    lhs /= scalar;
    return lhs;
}

[[nodiscard]] constexpr Mat2 transpose(const Mat2& m) noexcept
{
    return Mat2{
        m[0][0], m[1][0],
        m[0][1], m[1][1],
    };
}

[[nodiscard]] constexpr Mat2 operator*(const Mat2& lhs, const Mat2& rhs) noexcept
{
    Mat2 result{};
    for (std::size_t row = 0; row < 2; ++row) {
        for (std::size_t col = 0; col < 2; ++col) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < 2; ++k) {
                sum += lhs[row][k] * rhs[k][col];
            }
            result[row][col] = sum;
        }
    }
    return result;
}

[[nodiscard]] constexpr Vec2 operator*(const Mat2& lhs, const Vec2& v) noexcept
{
    return Vec2{
        lhs[0][0] * v.x() + lhs[0][1] * v.y(),
        lhs[1][0] * v.x() + lhs[1][1] * v.y(),
    };
}

