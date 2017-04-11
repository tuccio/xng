#pragma once

#include "math_fwd.hpp"
#include "matrix.hpp"

namespace xng
{
    namespace math
    {
        template <typename T>
        struct matrix<T, 3, 1> :
            matrix_traits<T, 3, 1>
        {

            matrix(void) = default;
            matrix(const matrix &) = default;

            matrix(scalar value) :
                x(value), y(value), z(value) {}

            matrix(scalar x, scalar y, scalar z) :
                x(x), y(y), z(z) {}

            matrix(const matrix<T, 2, 1> & xy, float z) :
                x(xy.m[0]), y(xy.m[1]), z(z) {}

            union
            {
                struct
                {
                    scalar x;
                    scalar y;
                    scalar z;
                };

                scalar m[3];
            };

            template <typename S>
            XNG_INLINE explicit operator matrix<S, 3, 1>() const
            {
                return matrix<S, 3, 1>((S)x, (S)y, (S)z);
            }

        };
    }
}