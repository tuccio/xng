#pragma once

#include "constants.hpp"

namespace xng
{
    namespace math
    {
        XNG_INLINE float to_radians(float x)
        {
            return x * 0.01745329251f;
        }

        XNG_INLINE float to_degrees(float x)
        {
            return x * 57.2957795131f;
        }

        template <typename T, int N, int M>
        XNG_INLINE matrix<T, N, M> to_radians(const matrix<T, N, M> & x)
        {
            return x * 0.01745329251f;
        }

        template <typename T, int N, int M>
        XNG_INLINE matrix<T, N, M> to_degrees(const matrix<T, N, M> & x)
        {
            return x * 57.2957795131f;
        }
    }
}