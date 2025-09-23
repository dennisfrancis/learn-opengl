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

}
