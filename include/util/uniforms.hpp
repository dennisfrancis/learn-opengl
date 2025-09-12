#pragma once

#include <glad/glad.h>
#include <string>

namespace util
{

struct Uniform
{
    GLint program;
    GLint location;
    std::string name;

    Uniform(const std::string& name_)
        : program{-1}
        , location{-1}
        , name{name_}
    {
    }
};

struct Uniform1f : public Uniform
{
    float value;

    Uniform1f(const std::string& name_, float vv)
        : value{ vv }
        , Uniform(name_)
    {
    }

    void set(float vv);
};

using mat4x4f_t = float[4][4];

struct Mat4x4f
{
    mat4x4f_t mat;
    Mat4x4f operator*(const Mat4x4f& other) const;
};

struct Matrix4f: public Uniform
{
    Matrix4f(const std::string& name_)
        : Uniform(name_)
    {
    }

    void set();
    void set(const mat4x4f_t& mat_);
    void set(const Mat4x4f& mat_);

    mat4x4f_t& get();
    float* get_ptr();

private:
    mat4x4f_t mat;
};

}
