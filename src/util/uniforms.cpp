#include <util/uniforms.hpp>

using namespace util;

void Uniform1f::set(float vv)
{
    value = vv;
    glUniform1f(location, value);
}
