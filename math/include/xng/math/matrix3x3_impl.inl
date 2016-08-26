#pragma once

namespace xng
{
	namespace math
	{
		XNG_INLINE float determinant(const float3x3 & lhs)
		{
			return lhs._00 * lhs._11 * lhs._22
				+ lhs._01 * lhs._12 * lhs._20
				+ lhs._02 * lhs._10 * lhs._21
				- lhs._00 * lhs._12 * lhs._21
				- lhs._01 * lhs._10 * lhs._22
				- lhs._02 * lhs._11 * lhs._20;
		}

		XNG_INLINE float3x3 inverse(const float3x3 & lhs, float * pDeterminant = nullptr)
		{
			float3x3 inv;

			inv._00 = lhs._11 * lhs._22 - lhs._12 * lhs._21;
			inv._01 = lhs._02 * lhs._21 - lhs._01 * lhs._22;
			inv._02 = lhs._01 * lhs._12 - lhs._02 * lhs._11;
			inv._10 = lhs._12 * lhs._20 - lhs._10 * lhs._22;
			inv._11 = lhs._00 * lhs._22 - lhs._02 * lhs._20;
			inv._12 = lhs._02 * lhs._10 - lhs._00 * lhs._12;
			inv._20 = lhs._10 * lhs._21 - lhs._11 * lhs._20;
			inv._21 = lhs._01 * lhs._20 - lhs._00 * lhs._21;
			inv._22 = lhs._00 * lhs._11 - lhs._01 * lhs._10;

			float det = determinant(lhs);
			assert(det != 0 && "Can't compute the inverse of a singular matrix.");

			float invDet = 1.f / det;

			if (pDeterminant)
			{
				*pDeterminant = det;
			}

			return inv * invDet;
		}
	}
}