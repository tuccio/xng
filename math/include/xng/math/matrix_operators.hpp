#pragma once

namespace xng
{
	namespace math
	{
		/* Matrix unary operators */

		template <typename T, int N, int M>
		matrix<T, N, M> operator- (const matrix<T, N, M> & a)
		{
			return detail::matrix_minus_impl<T, N, M>::minus(a);
		}

		/* Matrix/Matrix operators */

		template <typename T, int N, int M>
		matrix<T, N, M> operator+ (const matrix<T, N, M> & a, const matrix<T, N, M> & b)
		{
			return detail::matrix_add_impl<T, N, M>::add(a, b);
		}

		template <typename T, int N, int M>
		matrix<T, N, M> operator- (const matrix<T, N, M> & a, const matrix<T, N, M> & b)
		{
			return detail::matrix_sub_impl<T, N, M>::sub(a, b);
		}

		template <typename T, int N, int M, int K>
		matrix<T, N, M> operator* (const matrix<T, N, K> & a, const matrix<T, K, M> & b)
		{
			return detail::matrix_multiplication_impl<matrix<T, N, K>, matrix<T, K, M>, N, M>::multiply(a, b);
		}

		template <typename T, int N, int M>
		bool operator== (const matrix<T, N, M> & a, const matrix<T, N, M> & b)
		{
			return detail::matrix_comparison_impl<T, N, M>::eq(a, b);
		}

		/* Scalar/Matrix operators */

		template <typename T, int N, int M>
		matrix<T, N, M> operator* (T s, const matrix<T, N, M> & m)
		{
			return detail::matrix_scale_impl<T, N, M>::scale(m, s);
		}

		template <typename Scalar, typename T, int N, int M>
		std::enable_if_t<std::is_arithmetic<Scalar>::value, matrix<T, N, M>>
			operator* (const matrix<T, N, M> & m, Scalar s)
		{
			return detail::matrix_scale_impl<T, N, M>::scale(m, s);
		}

		template <typename T, int N, int M>
		std::enable_if_t<std::is_floating_point<T>::value, matrix<T, N, M>>
			operator/ (T s, const matrix<T, N, M> & m)
		{
			return detail::matrix_scale_impl<T, N, M>::scale(m, T(1) / s);
		}

		template <typename Scalar, typename T, int N, int M>
		std::enable_if_t<std::is_arithmetic<Scalar>::value, matrix<T, N, M>>
			operator/ (const matrix<T, N, M> & m, Scalar s)
		{
			return detail::matrix_scale_impl<T, N, M>::scale(m, T(1) / s);
		}

		template <typename T, int N, int M>
		std::enable_if_t<std::is_floating_point<T>::value, matrix<T, N, M>>
			operator/ (const matrix<T, N, M> & a, const matrix<T, N, M> & b)
		{
			return detail::matrix_scale_impl<T, N, M>::divide(a, b);
		}

		template <typename T, int N, int M>
		matrix<T, N, M> operator+ (T s, const matrix<T, N, M> & m)
		{
			return detail::matrix_add_impl<T, N, M>::add(m, s);
		}

		template <typename T, int N, int M>
		matrix<T, N, M> operator+ (const matrix<T, N, M> & m, T s)
		{
			return detail::matrix_add_impl<T, N, M>::add(m, s);
		}

		template <typename T, int N, int M>
		matrix<T, N, M> operator- (T s, const matrix<T, N, M> & m)
		{
			return detail::matrix_sub_impl<T, N, M>::sub(m, s);
		}

		template <typename T, int N, int M>
		matrix<T, N, M> operator- (const matrix<T, N, M> & m, T s)
		{
			return detail::matrix_sub_impl<T, N, M>::sub(m, s);
		}

		/* Vector operators */

		template <typename T, int N>
		std::enable_if_t<(N > 1), matrix<T, N, 1>> operator* (const matrix<T, N, 1> & a, const matrix<T, N, 1> & b)
		{
			return detail::matrix_scale_impl<T, N, 1>::scale(a, b);
		}

		template <typename T, int M>
		std::enable_if_t<(M > 1), matrix<T, 1, M>> operator* (const matrix<T, 1, M> & a, const matrix<T, 1, M> & b)
		{
			return detail::matrix_scale_impl<T, 1, M>::scale(a, b);
		}

	}
}