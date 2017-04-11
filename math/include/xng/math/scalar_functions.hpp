#pragma once

#include <type_traits>

namespace xng
{
    namespace math
    {
        /* Absolute value */

        XNG_INLINE float absolute(float a)
        {
            *(((int*)&a)) &= 0x7FFFFFFF;
            return a;
        }

        XNG_INLINE int absolute(int a)
        {
            // http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
            const int mask = a >> (sizeof(int) * CHAR_BIT - 1);
            return (a + mask) ^ mask;
        }

        /* Square root / Inverse */

#ifdef XNG_FAST_SQRT

        XNG_INLINE float inverse_square_root(float x)
        {
            float halfx = .5f * x;
            unsigned int i = *((unsigned int*)&x);
            i = 0x5f3759dfu - (i >> 1);
            float yi = *((float*)&i);
            float y0 = yi * (1.5f - (halfx * yi * yi));
            return y0;
        }

        template <typename T>
        XNG_INLINE std::enable_if_t<std::is_floating_point<T>::value, T> square_root(T x)
        {
            return x * inverse_square_root(x);
        }

#else

        XNG_INLINE float inverse_square_root(float x)
        {
            return 1.f / std::sqrt(x);
        }

        template <typename T>
        XNG_INLINE std::enable_if_t<std::is_floating_point<T>::value, T> square_root(T x)
        {
            return std::sqrt(x);
        }

#endif

        /* Min/Max */

        template <typename Scalar>
        XNG_INLINE std::enable_if_t<std::is_arithmetic<Scalar>::value, Scalar> min(Scalar a, Scalar b)
        {
            return a < b ? a : b;
        }

        template <typename Scalar>
        XNG_INLINE std::enable_if_t<std::is_arithmetic<Scalar>::value, Scalar> max(Scalar a, Scalar b)
        {
            return a > b ? a : b;
        }

    }
}