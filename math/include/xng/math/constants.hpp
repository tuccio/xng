#pragma once

namespace xng
{
    namespace math
    {
        namespace detail
        {
            template <typename T>
            struct constants;

            template <>
            struct constants<float>
            {
                constexpr static float pi = 3.1415927f;
            };

            template <>
            struct constants<double>
            {
                constexpr static double pi = 3.1415926535897;
            };
        }

        template <typename T = float>
        constexpr T pi(void)
        {
            return detail::constants<T>::pi;
        }
    }
}