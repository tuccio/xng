#pragma

#include <xng/math/scalar_functions.hpp>

namespace xng
{
	namespace math
	{
		namespace detail
		{
			/* Coefficient-wise functions */

			template <typename MatrixType, int I>
			struct matrix_cwise
			{
				using transpose_type = matrix<typename MatrixType::scalar, MatrixType::cols::value, MatrixType::rows::value>;

				XNG_INLINE static void minus(MatrixType & r, const MatrixType & a)
				{
					matrix_cwise<MatrixType, I - 1>::minus(r, a);
					get<I - 1>(r) = - get<I - 1>(a);
				}

				XNG_INLINE static void add(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::add(r, a, b);
					get<I - 1>(r) = get<I - 1>(a) + get<I - 1>(b);
				}

				template <typename Scalar>
				XNG_INLINE static void add(MatrixType & r, const MatrixType & a, Scalar s)
				{
					matrix_cwise<MatrixType, I - 1>::add(r, a, s);
					get<I - 1>(r) = get<I - 1>(a) + s;
				}

				XNG_INLINE static void sub(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::sub(r, a, b);
					get<I - 1>(r) = get<I - 1>(a) - get<I - 1>(b);
				}

				template <typename Scalar>
				XNG_INLINE static void sub(MatrixType & r, const MatrixType & a, Scalar s)
				{
					matrix_cwise<MatrixType, I - 1>::sub(r, a, s);
					get<I - 1>(r) = get<I - 1>(a) - s;
				}

				template <typename Scalar>
				XNG_INLINE static void sub(MatrixType & r, Scalar s, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::sub(r, s, b);
					get<I - 1>(r) = s - get<I - 1>(b);
				}

				template <typename Scalar>
				XNG_INLINE static void scale(MatrixType & r, const MatrixType & a, Scalar s)
				{
					matrix_cwise<MatrixType, I - 1>::scale(r, a, s);
					get<I - 1>(r) = get<I - 1>(a) * s;
				}

				XNG_INLINE static void scale(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::scale(r, a, b);
					get<I - 1>(r) = get<I - 1>(a) * get<I - 1>(b);
				}

				XNG_INLINE static void divide(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::divide(r, a, b);
					get<I - 1>(r) = get<I - 1>(a) / get<I - 1>(b);

				}
				XNG_INLINE static void transpose(transpose_type & r, const MatrixType & a)
				{
					matrix_cwise<MatrixType, I - 1>::transpose(r, a);
					using _row = std::integral_constant<int, (I - 1) % MatrixType::cols::value>;
					using _col = std::integral_constant<int, (I - 1) / MatrixType::cols::value>;
					get<_row::value, _col::value>(r) = get<_col::value, _row::value>(a);
				}

				XNG_INLINE static void hadd(typename MatrixType::scalar & r, const MatrixType & a)
				{
					matrix_cwise<MatrixType, I - 1>::hadd(r, a);
					r += get<I - 1>(a);
				}

				XNG_INLINE static void min(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::min(r, a, b);
					get<I - 1>(r) = xng::math::min(get<I - 1>(a), get<I - 1>(b));
				}

				XNG_INLINE static void max(MatrixType & r, const MatrixType & a, const MatrixType & b)
				{
					matrix_cwise<MatrixType, I - 1>::max(r, a, b);
					get<I - 1>(r) = xng::math::max(get<I - 1>(a), get<I - 1>(b));
				}

				XNG_INLINE static void square_root(typename MatrixType & r, const MatrixType & a)
				{
					matrix_cwise<MatrixType, I - 1>::square_root(r, a);
					get<I - 1>(r) = xng::math::square_root(get<I - 1>(a), get<I - 1>(b));
				}

				XNG_INLINE static void inverse_square_root(typename MatrixType & r, const MatrixType & a)
				{
					matrix_cwise<MatrixType, I - 1>::inverse_square_root(r, a);
					get<I - 1>(r) = xng::math::inverse_square_root(get<I - 1>(a), get<I - 1>(b));
				}
			};

			template <typename MatrixType>
			struct matrix_cwise<MatrixType, 0>
			{
				using transpose_type = matrix<typename MatrixType::scalar, MatrixType::cols::value, MatrixType::rows::value>;

				XNG_INLINE static void minus(MatrixType & r, const MatrixType & a) {}

				XNG_INLINE static void add(MatrixType & r, const MatrixType & a, const MatrixType & b) {}

				template <typename Scalar>
				XNG_INLINE static void add(MatrixType & r, const MatrixType & a, Scalar s) {}

				XNG_INLINE static void sub(MatrixType & r, const MatrixType & a, const MatrixType & b) {}

				template <typename Scalar>
				XNG_INLINE static void sub(MatrixType & r, const MatrixType & a, Scalar s) {}

				template <typename Scalar>
				XNG_INLINE static void sub(MatrixType & r, Scalar s, const MatrixType & b) {}

				template <typename Scalar>
				XNG_INLINE static void scale(MatrixType & r, const MatrixType & a, Scalar s) {}

				XNG_INLINE static void scale(MatrixType & r, const MatrixType & a, const MatrixType & b) {}
				XNG_INLINE static void divide(MatrixType & r, const MatrixType & a, const MatrixType & b) {}

				XNG_INLINE static void transpose(transpose_type & r, const MatrixType & a) {}

				XNG_INLINE static void hadd(typename MatrixType::scalar & r, const MatrixType & a) { r = typename MatrixType::scalar(0); }

				XNG_INLINE static void min(MatrixType & r, const MatrixType & a, const MatrixType & b) {}
				XNG_INLINE static void max(MatrixType & r, const MatrixType & a, const MatrixType & b) {}

				XNG_INLINE static void square_root(typename MatrixType & r, const MatrixType & a) {}
				XNG_INLINE static void inverse_square_root(typename MatrixType & r, const MatrixType & a) {}

			};

			/* Coefficient-wise implementations */

			template <typename T, int N, int M>
			struct matrix_minus_impl
			{
				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type minus(const matrix_type & a)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::minus(r, a);
					return r;
				}
			};

			template <typename T, int N, int M>
			struct matrix_add_impl
			{
				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type add(const matrix_type & a, const matrix_type & b)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::add(r, a, b);
					return r;
				}
				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type add(const matrix_type & a, T s)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::add(r, a, s);
					return r;
				}

				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type add(T s, const matrix_type & b)
				{
					return add(b, s);
				}
			};

			template <typename T, int N, int M>
			struct matrix_sub_impl
			{
				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type sub(const matrix_type & a, const matrix_type & b)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::sub(r, a, b);
					return r;
				}

				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type sub(const matrix_type & a, T s)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::sub(r, a, s);
					return r;
				}

				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static matrix_type sub(T s, const matrix_type & b)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::sub(r, s, b);
					return r;
				}
			};

			template <typename T, int N, int M>
			struct matrix_comparison_impl
			{
				using matrix_type = matrix<T, N, M>;
				XNG_INLINE static bool eq(const matrix_type & a, const matrix_type & b)
				{
					return memcmp(a.m, b.m, N * M * sizeof(typename matrix_type::scalar)) == 0;
				}
			};

			template <typename T, int N, int M>
			struct matrix_scale_impl
			{
				using matrix_type = matrix<T, N, M>;

				template <typename Scalar>
				XNG_INLINE static std::enable_if_t<std::is_arithmetic<Scalar>::value, matrix_type> scale(const matrix_type & a, Scalar s)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::scale(r, a, s);
					return r;
				}

				XNG_INLINE static matrix_type scale(const matrix_type & a, const matrix_type & b)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::scale(r, a, b);
					return r;
				}

				XNG_INLINE static matrix_type divide(const matrix_type & a, const matrix_type & b)
				{
					matrix_type r;
					matrix_cwise<matrix_type, N * M>::divide(r, a, b);
					return r;
				}
			};

			template <typename T, int N, int M>
			struct matrix_transpose_impl
			{
				using matrix_type    = matrix<T, N, M>;
				using transpose_type = matrix<T, M, N>;

				XNG_INLINE static transpose_type transpose(const matrix_type & a)
				{
					transpose_type r;
					matrix_cwise<matrix_type, N * M>::transpose(r, a);
					return r;
				}
			};

			template <typename T>
			struct matrix_transpose_impl<T, 1, 1>
			{
				using matrix_type = matrix<T, 1, 1>;
				using transpose_type = matrix<T, 1, 1>;

				XNG_INLINE static const transpose_type & transpose(const matrix_type & a)
				{
					return a;
				}
			};

			template <typename T, int M>
			struct matrix_transpose_impl<T, 1, M>
			{
				using matrix_type    = matrix<T, 1, M>;
				using transpose_type = matrix<T, M, 1>;

				XNG_INLINE static const transpose_type & transpose(const matrix_type & a)
				{
					return reinterpret_cast<const transpose_type&>(a);
				}
			};

			template <typename T, int N>
			struct matrix_transpose_impl<T, N, 1>
			{
				using matrix_type    = matrix<T, N, 1>;
				using transpose_type = matrix<T, 1, N>;

				XNG_INLINE static const transpose_type & transpose(const matrix_type & a)
				{
					return reinterpret_cast<const transpose_type&>(a);
				}
			};

			/* Fill (accesses I, J by column) */

			template <typename MatrixType, int I, int J>
			struct matrix_fill_impl
			{
				XNG_INLINE static void fill(MatrixType & r, typename MatrixType::scalar d, typename MatrixType::scalar f)
				{
					matrix_fill_impl<MatrixType, I - 1, J>::fill(r, d, f);
					get<I - 1, J - 1>(r) = (I == J ? d : f);
				}
			};

			template <typename MatrixType, int J>
			struct matrix_fill_impl<MatrixType, 0, J>
			{
				XNG_INLINE static void fill(MatrixType & r, typename MatrixType::scalar d, typename MatrixType::scalar f)
				{
					matrix_fill_impl<MatrixType, MatrixType::rows::value, J - 1>::fill(r, d, f);
				}
			};

			template <typename MatrixType, int I>
			struct matrix_fill_impl<MatrixType, I, 0>
			{
				XNG_INLINE static void fill(MatrixType & r, typename MatrixType::scalar d, typename MatrixType::scalar f) {}
			};

			/* Row-Column product */

			template <typename MatrixType1, typename MatrixType2, int I, int J, int K>
			struct matrix_row_col_prod_impl
			{
				XNG_INLINE static void multiply(typename MatrixType1::scalar & r, const MatrixType1 & a, const MatrixType2 & b)
				{
					matrix_row_col_prod_impl<MatrixType1, MatrixType2, I, J, K - 1>::multiply(r, a, b);
					r += get<I, K - 1>(a) * get<K - 1, J>(b);
					//std::cout << "(" << I << ", " << J << ") += " << get<I, K - 1>(a) << " * " << get<K - 1, J>(b) << std::endl;
				}
			};

			template <typename MatrixType1, typename MatrixType2, int I, int J>
			struct matrix_row_col_prod_impl<MatrixType1, MatrixType2, I, J, 0>
			{
				XNG_INLINE static void multiply(typename MatrixType1::scalar & r, const MatrixType1 & a, const MatrixType2 & b)
				{
					r = 0;
				}
			};

			/* Matrix multiplication */

			template <typename MatrixType1, typename MatrixType2, int I, int J>
			struct matrix_multiplication_impl
			{
				using result_type = matrix<typename MatrixType1::scalar, MatrixType1::rows::value, MatrixType2::cols::value>;

				static result_type multiply(const MatrixType1 & a, const MatrixType2 & b)
				{
					result_type r;
					multiply(r, a, b);
					return r;
				}

				XNG_INLINE static void multiply(result_type & r, const MatrixType1 & a, const MatrixType2 & b)
				{
					matrix_multiplication_impl<MatrixType1, MatrixType2, I - 1, J>::multiply(r, a, b);
					matrix_row_col_prod_impl<MatrixType1, MatrixType2, I - 1, J - 1, MatrixType1::cols::value>::multiply(get<I - 1, J - 1>(r), a, b);
				}
			};

			template <typename MatrixType1, typename MatrixType2, int J>
			struct matrix_multiplication_impl<MatrixType1, MatrixType2, 0, J>
			{
				using result_type = matrix<typename MatrixType1::scalar, MatrixType1::rows::value, MatrixType2::cols::value>;

				XNG_INLINE static void multiply(result_type & r, const MatrixType1 & a, const MatrixType2 & b)
				{
					matrix_multiplication_impl<MatrixType1, MatrixType2, result_type::rows::value, J - 1>::multiply(r, a, b);
				}
			};

			template <typename MatrixType1, typename MatrixType2, int I>
			struct matrix_multiplication_impl<MatrixType1, MatrixType2, I, 0>
			{
				using result_type = matrix<typename MatrixType1::scalar, MatrixType1::rows::value, MatrixType2::cols::value>;

				XNG_INLINE static void multiply(result_type & r, const MatrixType1 & a, const MatrixType2 & b) {}
			};
		}
	}
}