#pragma once

#include "math_fwd.hpp"
#include "matrix.hpp"

namespace xng
{
	namespace math
	{
		template <typename T>
		struct matrix<T, 2, 2> :
			matrix_traits<T, 2, 2>
		{

			matrix(void) = default;
			matrix(const matrix &) = default;

			matrix(
				scalar m00, scalar m01,
				scalar m10, scalar m11) :
				_00(m00), _10(m10),
				_01(m01), _11(m11) {}

			matrix(T d) :
				matrix(
					d, 0,
					0, d) {}

			union
			{

				struct
				{
					matrix<T, 1, 2> r[3];
				};

				scalar m[4];

				struct
				{
					scalar _00;
					scalar _01;

					scalar _10;
					scalar _11;
				};

			};

		};
	}
}