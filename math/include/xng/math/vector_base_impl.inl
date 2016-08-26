#pragma once

namespace xng
{
	namespace math
	{
		namespace detail
		{
			/* Coefficient-wise implementations */		

			template <typename VectorType, int N>
			struct vector_dot_impl
			{
				using scalar     = typename VectorType::scalar;
				using col_vector = matrix<typename VectorType::scalar, N, 1>;
				using row_vector = matrix<typename VectorType::scalar, 1, N>;

				XNG_INLINE static std::enable_if_t<std::is_same<VectorType, col_vector>::value || std::is_same<VectorType, row_vector>::value, scalar>
					dot(const VectorType & a, const VectorType & b)
				{
					scalar r;
					matrix_cwise<VectorType, N>::hadd(r, scale(a, b));
					return r;
				}
			};

			template <typename VectorType, int N>
			struct vector_length_impl
			{
				XNG_INLINE static float length(const VectorType & a)
				{
					return square_root(dot(a, a));
				}
			};

			template <typename VectorType, int N>
			struct vector_min_max_impl
			{
				using scalar = typename VectorType::scalar;
				using col_vector = matrix<typename VectorType::scalar, N, 1>;
				using row_vector = matrix<typename VectorType::scalar, 1, N>;

				XNG_INLINE static std::enable_if_t<std::is_same<VectorType, col_vector>::value || std::is_same<VectorType, row_vector>::value, VectorType>
					min(const VectorType & a, const VectorType & b)
				{
					VectorType r;
					matrix_cwise<VectorType, N>::min(r, a, b);
					return r;
				}

				XNG_INLINE static std::enable_if_t<std::is_same<VectorType, col_vector>::value || std::is_same<VectorType, row_vector>::value, VectorType>
					max(const VectorType & a, const VectorType & b)
				{
					VectorType r;
					matrix_cwise<VectorType, N>::max(r, a, b);
					return r;
				}
			};
		}
	}
}