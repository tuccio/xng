#pragma once

namespace xng
{
    namespace math
    {
        template <int I, typename T, int N, int M>
        XNG_INLINE T & get(matrix<T, N, M> & x)
        {
            return x.m[I];
        }

        template <int I, typename T, int N, int M>
        XNG_INLINE const T & get(const matrix<T, N, M> & x)
        {
            return x.m[I];
        }

        template <int I, int J, typename T, int N, int M>
        XNG_INLINE T & get(matrix<T, N, M> & x)
        {
            return XNG_MATH_ROW_MAJOR_ACCESS(x.m, N, M, I, J);
        }

        template <int I, int J, typename T, int N, int M>
        XNG_INLINE const T & get(const matrix<T, N, M> & x)
        {
            return XNG_MATH_ROW_MAJOR_ACCESS(x.m, N, M, I, J);
        }

        /* Functions */

        template <typename T, int N, int M>
        XNG_INLINE void fill(matrix<T, N, M> & m, T d, T f = 0)
        {
            detail::matrix_fill_impl<matrix<T, N, M>, N, M>::fill(m, d, f);
        }

        template <typename T, int N, int M>
        XNG_INLINE matrix<T, N, M> scale(const matrix<T, N, M> & a, const matrix<T, N, M> & b)
        {
            return detail::matrix_scale_impl<T, N, M>::scale(a, b);
        }

        template <typename T, int N, int M>
        XNG_INLINE decltype(auto) transpose(const matrix<T, N, M> & a)
        {
            return detail::matrix_transpose_impl<T, N, M>::transpose(a);
        }
    }
}