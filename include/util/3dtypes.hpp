
namespace util
{
using mat4x4f_t = float[4][4];

struct Mat4x4f
{
    mat4x4f_t mat;
    Mat4x4f operator*(const Mat4x4f& other) const;
};
}
