#pragma once

#include <numbers>

namespace util
{

constexpr inline float to_radian(float x) { return x * std::numbers::pi_v<float> / 180.0f; }

constexpr inline float to_degree(float x) { return x * 180.0f / std::numbers::pi_v<float>; }

/// Returns random float in the range of [0.0, 1.0]
float random_float();

using mat4x4f_t = float[4][4];

struct Mat4x4f
{
    mat4x4f_t mat;
    Mat4x4f() {};
    Mat4x4f(
        // row 0
        float a00, float a01, float a02, float a03,
        // row 1
        float a10, float a11, float a12, float a13,
        // row 2
        float a20, float a21, float a22, float a23,
        // row 3
        float a30, float a31, float a32, float a33);
    Mat4x4f operator*(const Mat4x4f& other) const;

    void init_scale_transform(float scale_x, float scale_y, float scale_z);
    void init_scale_transform(float scale);
    void init_scale_transform(float scales[3]);

    void init_rotate_transform(float rotate_x, float rotate_y, float rotate_z);
    void init_rotate_transform_zyx(float rotate_x, float rotate_y, float rotate_z);
    void init_rotate_transform_x(float x);
    void init_rotate_transform_y(float y);
    void init_rotate_transform_z(float z);

    void init_translation_transform(float x, float y, float z);
};

}
