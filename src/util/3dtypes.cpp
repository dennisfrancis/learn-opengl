#include <util/3dtypes.hpp>
#include <random>

namespace util
{

namespace
{
std::random_device rd; // Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> unirand(0.0, 1.0);
}

float random_float()
{
    return unirand(gen);
}

Mat4x4f::Mat4x4f(
            // row 0
            float a00, float a01, float a02, float a03,
            // row 1
            float a10, float a11, float a12, float a13,
            // row 2
            float a20, float a21, float a22, float a23,
            // row 3
            float a30, float a31, float a32, float a33)
{
    mat[0][0] = a00;
    mat[0][1] = a01;
    mat[0][2] = a02;
    mat[0][3] = a03;

    mat[1][0] = a10;
    mat[1][1] = a11;
    mat[1][2] = a12;
    mat[1][3] = a13;

    mat[2][0] = a20;
    mat[2][1] = a21;
    mat[2][2] = a22;
    mat[2][3] = a23;

    mat[3][0] = a30;
    mat[3][1] = a31;
    mat[3][2] = a32;
    mat[3][3] = a33;

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

void Mat4x4f::init_scale_transform(float scale_x, float scale_y, float scale_z)
{
    float scales[3]{scale_x, scale_y, scale_z};
    init_scale_transform(scales);
}

void Mat4x4f::init_scale_transform(float scale)
{
    float scales[3]{scale, scale, scale};
    init_scale_transform(scales);
}

void Mat4x4f::init_scale_transform(float scales[3])
{
    for (short ii = 0; ii < 4; ++ii)
    {
        for (short jj = 0; jj < 4; ++jj)
        {
            if (ii == jj)
            {
                mat[ii][ii] = (ii < 3) ? scales[ii] : 1.0f;
            }
            else
            {
                mat[ii][jj] = 0.0f;
            }
        }
    }
}

namespace
{

void fill_deg_rotations(Mat4x4f& rx, Mat4x4f& ry, Mat4x4f& rz, float x, float y, float z)
{
    x = to_radian(x);
    y = to_radian(y);
    z = to_radian(z);

    rx.init_rotate_transform_x(x);
    ry.init_rotate_transform_y(y);
    rz.init_rotate_transform_z(z);
}

} // end of anonymous namespace

void Mat4x4f::init_rotate_transform(float rotate_x, float rotate_y, float rotate_z)
{
    Mat4x4f rx, ry, rz;
    fill_deg_rotations(rx, ry, rz, rotate_x, rotate_y, rotate_z);

    *this = rz * ry * rx;
}

void Mat4x4f::init_rotate_transform_zyx(float rotate_x, float rotate_y, float rotate_z)
{

    Mat4x4f rx, ry, rz;

    fill_deg_rotations(rx, ry, rz, rotate_x, rotate_y, rotate_z);

    *this = rx * ry * rz;
}

void Mat4x4f::init_rotate_transform_x(float x)
{
    mat[0][0] = 1.0f;
    mat[0][1] = 0.0f;
    mat[0][2] = 0.0f;
    mat[0][3] = 0.0f;

    mat[1][0] = 0.0f;
    mat[1][1] = cosf(x);
    mat[1][2] = sinf(x);
    mat[1][3] = 0.0f;

    mat[2][0] = 0.0f;
    mat[2][1] = -sinf(x);
    mat[2][2] = cosf(x);
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
}

void Mat4x4f::init_rotate_transform_y(float y)
{
    mat[0][0] = cosf(y);
    mat[0][1] = 0.0f;
    mat[0][2] = -sinf(y);
    mat[0][3] = 0.0f;

    mat[1][0] = 0.0f;
    mat[1][1] = 1.0f;
    mat[1][2] = 0.0f;
    mat[1][3] = 0.0f;

    mat[2][0] = sinf(y);
    mat[2][1] = 0.0f;
    mat[2][2] = cosf(y);
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
}

void Mat4x4f::init_rotate_transform_z(float z)
{
    mat[0][0] = cosf(z);
    mat[0][1] = sinf(z);
    mat[0][2] = 0.0f;
    mat[0][3] = 0.0f;

    mat[1][0] = -sinf(z);
    mat[1][1] = cosf(z);
    mat[1][2] = 0.0f;
    mat[1][3] = 0.0f;

    mat[2][0] = 0.0f;
    mat[2][1] = 0.0f;
    mat[2][2] = 1.0f;
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
}

}
