#include <cstring>
#include <util/uniforms.hpp>

using namespace util;

void Uniform1f::set(float vv)
{
    value = vv;
    glUniform1f(location, value);
}

Mat4x4f Mat4x4f::operator*(const Mat4x4f& other) const
{
    Mat4x4f res;
    for (int ii = 0; ii < 4; ++ii)
    {
        for (int jj = 0; jj < 4; ++jj)
        {
            float dot = 0.0f;
            for (int kk = 0; kk < 4; ++kk)
            {
                dot += mat[ii][kk] * other.mat[kk][jj];
            }
            res.mat[ii][jj] = dot;
        }
    }

    return res;
}

void Matrix4f::set()
{
    glUniformMatrix4fv(location, 1, GL_TRUE /* already in row major form */, get_ptr());
}

void Matrix4f::set(const mat4x4f_t& mat_)
{
    float* base = get_ptr();
    std::memcpy(base, &mat_[0][0], sizeof(float) * 16);
    set();
}

void Matrix4f::set(const Mat4x4f& mat_)
{
    float* base = get_ptr();
    std::memcpy(base, &mat_.mat[0][0], sizeof(float) * 16);
    set();
}

mat4x4f_t& Matrix4f::get() { return mat; }

float* Matrix4f::get_ptr() { return &mat[0][0]; }
