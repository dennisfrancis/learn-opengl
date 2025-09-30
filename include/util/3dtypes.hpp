#pragma once

#include <numbers>
#include <cmath>

namespace util
{

constexpr inline float to_radian(float x) { return x * std::numbers::pi_v<float> / 180.0f; }

constexpr inline float to_degree(float x) { return x * 180.0f / std::numbers::pi_v<float>; }

/// Returns random float in the range of [0.0, 1.0]
float random_float();

struct Vec3f
{
    union
    {
        float x = 0.0f;
        float r;
    };

    union
    {
        float y = 0.0f;
        float g;
    };

    union
    {
        float z = 0.0f;
        float b;
    };

    Vec3f() {}

    Vec3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vec3f(const float* float_arr)
    {
        x = float_arr[0];
        y = float_arr[1];
        z = float_arr[2];
    }

    Vec3f(float f) { x = y = z = f; }

    Vec3f& operator+=(const Vec3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vec3f& operator-=(const Vec3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vec3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    bool operator==(const Vec3f& r) { return ((x == r.x) && (y == r.y) && (z == r.z)); }

    bool operator!=(const Vec3f& r) { return !(*this == r); }

    Vec3f cross(const Vec3f& v) const;

    float dot(const Vec3f& v) const
    {
        float ret = x * v.x + y * v.y + z * v.z;
        return ret;
    }

    float distance(const Vec3f& v) const
    {
        float delta_x = x - v.x;
        float delta_y = y - v.y;
        float delta_z = z - v.z;
        float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
        return distance;
    }

    float length() const
    {
        float len = sqrtf(x * x + y * y + z * z);
        return len;
    }

    bool is_zero() const { return ((x + y + z) == 0.0f); }

    Vec3f& normalize();
};

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
