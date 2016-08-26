#pragma once

namespace xng
{
	namespace math
	{
		namespace detail
		{
			template <typename T>
			struct is_matrix :
				std::false_type {};

			template <typename T, int N, int M>
			struct is_matrix<matrix<T, N, M>> :
				std::true_type {};

			template <typename T>
			struct is_row_vector :
				std::false_type {};

			template <typename T, int M>
			struct is_row_vector<matrix<T, 1, M>> :
				std::true_type {};

			template <typename T>
			struct is_column_vector :
				std::false_type {};

			template <typename T, int N>
			struct is_column_vector<matrix<T, N, 1>> :
				std::true_type {};

			template <typename T>
			struct is_vector :
				std::integral_constant<bool, is_row_vector<T>::value || is_column_vector<T>::value> {};
		}
	}
}