#include <util/uniforms.hpp>

using namespace util;

void Uniform1f::set(float vv)
{
    value = vv;
    glUniform1f(location, value);
}

void Matrix4f::set()
{
    glUniformMatrix4fv(location, 1, GL_TRUE /* already in row major form */, &mat[0][0]);
}
