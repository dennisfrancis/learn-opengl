#include <util/3dtypes.hpp>

using namespace util;

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
