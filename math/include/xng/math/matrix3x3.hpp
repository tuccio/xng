#pragma once

#include "math_fwd.hpp"
#include "matrix.hpp"

namespace xng
{
    namespace math
    {
        template <typename T>
        struct matrix<T, 3, 3> :
            matrix_traits<T, 3, 3>
        {

            matrix(void) = default;
            matrix(const matrix &) = default;

            matrix(
                scalar m00, scalar m01, scalar m02,
                scalar m10, scalar m11, scalar m12,
                scalar m20, scalar m21, scalar m22) :
                _00(m00), _10(m10), _20(m20),
                _01(m01), _11(m11), _21(m21),
                _02(m02), _12(m12), _22(m22) {}

            matrix(T d) :
                matrix(
                    d, 0, 0,
                    0, d, 0,
                    0, 0, d) {}

            union
            {

                struct
                {
                    matrix<T, 1, 3> r[3];
                };

                scalar m[9];

                struct
                {
                    scalar _00;
                    scalar _01;
                    scalar _02;

                    scalar _10;
                    scalar _11;
                    scalar _12;

                    scalar _20;
                    scalar _21;
                    scalar _22;
                };
            };
        };
    }
}