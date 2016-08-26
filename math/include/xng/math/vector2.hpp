#pragma once

#include "math_fwd.hpp"
#include "matrix.hpp"

namespace xng
{
	namespace math
	{
		template <typename T>
		struct matrix<T, 2, 1> :
			matrix_traits<T, 2, 1>
		{

			matrix(void) = default;
			matrix(const matrix &) = default;

			matrix(scalar value) :
				x(value), y(value) {}

			matrix(scalar x, scalar y) :
				x(x), y(y) {}

			union
			{
				struct
				{
					scalar x;
					scalar y;
				};

				scalar m[2];
			};

		};
	}
}