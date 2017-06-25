#pragma once

#include "forward.hpp"
#include "matrix.hpp"

namespace xng
{
    namespace math
    {
        template <typename T>
        struct matrix<T, 4, 1> :
            matrix_traits<T, 4, 1>
        {

            matrix(void) = default;
            matrix(const matrix &) = default;

            matrix(scalar value) :
                x(value), y(value), z(value), w(value) {}

            matrix(scalar x, scalar y, scalar z, scalar w) :
                x(x), y(y), z(z), w(w) {}

            matrix(const matrix<T, 3, 1> & xyz, float w) :
                x(xyz.m[0]), y(xyz.m[1]), z(xyz.m[2]), w(w) {}

            union
            {

                struct
                {
                    scalar x;
                    scalar y;
                    scalar z;
                    scalar w;
                };

                scalar m[4];

            };

            template <typename S>
            XNG_INLINE explicit operator matrix<S, 4, 1>() const
            {
                return matrix<S, 4, 1>((S)x, (S)y, (S)z, (S)w);
            }

        };
    }
}