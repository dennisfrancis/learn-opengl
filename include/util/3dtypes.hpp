#pragma once

#include <numbers>

namespace util
{

constexpr inline float to_radian(float x)
{
    return x * std::numbers::pi_v<float> / 180.0f;
}

constexpr inline float to_degree(float x)
{
    return x * 180.0f / std::numbers::pi_v<float>;
}

/// Returns random float in the range of [0.0, 1.0]
float random_float();

using mat4x4f_t = float[4][4];

struct Mat4x4f
{
    mat4x4f_t mat;
    Mat4x4f operator*(const Mat4x4f& other) const;
};

}
