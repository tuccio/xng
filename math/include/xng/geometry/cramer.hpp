#pragma once

#include <xng/math.hpp>

#include <cassert>

namespace xng
{
	namespace math
	{
		XNG_INLINE float3 cramer3(const float3x3 & A, const float3 & b)
		{
			float det = determinant(A);
			assert(det != 0 && "Singular matrix means the system has infinite solutions.");
			float invDet = 1.f / det;

			float3row bc = transpose(b);

			float3 t;
			t.x = invDet * dot(bc, cross(A.r[1], A.r[2]));
			t.y = invDet * dot(A.r[1], cross(bc, A.r[2]));
			t.z = invDet * dot(A.r[1], cross(A.r[1], bc));
			return t;
		}
	}
}