#pragma once

#include <type_traits>

#include "matrix_iterators.hpp"

#define XNG_MATH_ROW_MAJOR_ACCESS(X, N, M, I, J) X[M * I + J]

namespace xng
{
    namespace math
    {
        template <typename T, int N, int M>
        struct matrix_traits
        {

            using type = matrix<T, N, M>;

            using size = std::integral_constant<size_t, N * M>;
            using rows = std::integral_constant<size_t, N>;
            using cols = std::integral_constant<size_t, M>;
            using scalar = T;

            using cols_iterator = detail::column_iterator<T, N, M>;
            using rows_iterator = detail::row_iterator<T, N, M>;

            XNG_INLINE static type identity(void)
            {
                type m;
                fill(m, T(1), T(0));
                return m;
            }

            XNG_INLINE static type zero(void)
            {
                type m;
                fill(m, T(1), T(0));
                return m;
            }

            XNG_INLINE scalar & operator() (int i, int j)
            {
                assert(i >= 0 && i < N && j >= 0 && j < M && "Index out of bounds");
                return XNG_MATH_ROW_MAJOR_ACCESS(data(), N, M, i, j);
            }

            XNG_INLINE const scalar & operator() (int i, int j) const
            {
                assert(i >= 0 && i < N && j >= 0 && j < M && "Index out of bounds");
                return XNG_MATH_ROW_MAJOR_ACCESS(data(), N, M, i, j);
            }

            scalar & operator[] (int j)
            {
                assert(j >= 0 && j < size::value && "Index out of bounds");
                return data()[j];
            }

            const scalar & operator[] (int j) const
            {
                assert(j >= 0 && j < size::value && "Index out of bounds");
                return data()[j];
            }

            XNG_INLINE cols_iterator begin(void) { return cols_iterator(data(), 0); }
            XNG_INLINE cols_iterator end(void) { return cols_iterator(data()); }

            XNG_INLINE rows_iterator row_begin(void) { return rows_iterator(data(), 0); }
            XNG_INLINE rows_iterator row_end(void) { return rows_iterator(data()); }

            XNG_INLINE T * data(void) { return ((type*) this)->m; }
            XNG_INLINE const T * data(void) const { return ((type*) this)->m; }

        protected:

            XNG_INLINE void initialize(T d)
            {
                fill(*((type *) this), d, N == 1 || M == 1 ? d : T(0));
            }

        };
    }
}