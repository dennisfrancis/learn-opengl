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

struct Matrix4f: public Uniform
{
    float mat[4][4];

    Matrix4f(const std::string& name_)
        : Uniform(name_)
    {
    }

    void set();
};

}
