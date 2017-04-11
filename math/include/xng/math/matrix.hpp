#pragma once

#include "matrix_traits.hpp"

namespace xng
{
    namespace math
    {
        template <typename T, int N, int M>
        struct matrix :
            matrix_traits<T, N, M>
        {

            matrix(void) = default;
            matrix(const matrix &) = default;

            matrix(T d)
            {
                initialize(d);
            }

            matrix(std::initializer_list<scalar> init)
            {
                std::copy(init.begin(), init.end(), row_begin());
            }

            T m[M * N];

        };
    }
}