#pragma once

namespace xng
{
	namespace math
	{
		XNG_INLINE float determinant(const float4x4 & lhs)
		{
			return lhs._03 * lhs._12 * lhs._21 * lhs._30
				- lhs._02 * lhs._13 * lhs._21 * lhs._30
				- lhs._03 * lhs._11 * lhs._22 * lhs._30
				+ lhs._01 * lhs._13 * lhs._22 * lhs._30
				+ lhs._02 * lhs._11 * lhs._23 * lhs._30
				- lhs._01 * lhs._12 * lhs._23 * lhs._30
				- lhs._03 * lhs._12 * lhs._20 * lhs._31
				+ lhs._02 * lhs._13 * lhs._20 * lhs._31
				+ lhs._03 * lhs._10 * lhs._22 * lhs._31
				- lhs._00 * lhs._13 * lhs._22 * lhs._31
				- lhs._02 * lhs._10 * lhs._23 * lhs._31
				+ lhs._00 * lhs._12 * lhs._23 * lhs._31
				+ lhs._03 * lhs._11 * lhs._20 * lhs._32
				- lhs._01 * lhs._13 * lhs._20 * lhs._32
				- lhs._03 * lhs._10 * lhs._21 * lhs._32
				+ lhs._00 * lhs._13 * lhs._21 * lhs._32
				+ lhs._01 * lhs._10 * lhs._23 * lhs._32
				- lhs._00 * lhs._11 * lhs._23 * lhs._32
				- lhs._02 * lhs._11 * lhs._20 * lhs._33
				+ lhs._01 * lhs._12 * lhs._20 * lhs._33
				+ lhs._02 * lhs._10 * lhs._21 * lhs._33
				- lhs._00 * lhs._12 * lhs._21 * lhs._33
				- lhs._01 * lhs._10 * lhs._22 * lhs._33
				+ lhs._00 * lhs._11 * lhs._22 * lhs._33;
		}

		XNG_INLINE float4x4 inverse(const float4x4 & lhs, float * pDeterminant = nullptr)
		{
			float4x4 inv;

			inv._00 = lhs._11  * lhs._22 * lhs._33 -
				lhs._11  * lhs._23 * lhs._32 -
				lhs._21  * lhs._12  * lhs._33 +
				lhs._21  * lhs._13  * lhs._32 +
				lhs._31 * lhs._12  * lhs._23 -
				lhs._31 * lhs._13  * lhs._22;

			inv._10 = -lhs._10  * lhs._22 * lhs._33 +
				lhs._10  * lhs._23 * lhs._32 +
				lhs._20  * lhs._12  * lhs._33 -
				lhs._20  * lhs._13  * lhs._32 -
				lhs._30 * lhs._12  * lhs._23 +
				lhs._30 * lhs._13  * lhs._22;

			inv._20 = lhs._10  * lhs._21 * lhs._33 -
				lhs._10  * lhs._23 * lhs._31 -
				lhs._20  * lhs._11 * lhs._33 +
				lhs._20  * lhs._13 * lhs._31 +
				lhs._30 * lhs._11 * lhs._23 -
				lhs._30 * lhs._13 * lhs._21;

			inv._30 = -lhs._10  * lhs._21 * lhs._32 +
				lhs._10  * lhs._22 * lhs._31 +
				lhs._20  * lhs._11 * lhs._32 -
				lhs._20  * lhs._12 * lhs._31 -
				lhs._30 * lhs._11 * lhs._22 +
				lhs._30 * lhs._12 * lhs._21;

			inv._01 = -lhs._01  * lhs._22 * lhs._33 +
				lhs._01  * lhs._23 * lhs._32 +
				lhs._21  * lhs._02 * lhs._33 -
				lhs._21  * lhs._03 * lhs._32 -
				lhs._31 * lhs._02 * lhs._23 +
				lhs._31 * lhs._03 * lhs._22;

			inv._11 = lhs._00  * lhs._22 * lhs._33 -
				lhs._00  * lhs._23 * lhs._32 -
				lhs._20  * lhs._02 * lhs._33 +
				lhs._20  * lhs._03 * lhs._32 +
				lhs._30 * lhs._02 * lhs._23 -
				lhs._30 * lhs._03 * lhs._22;

			inv._21 = -lhs._00  * lhs._21 * lhs._33 +
				lhs._00  * lhs._23 * lhs._31 +
				lhs._20  * lhs._01 * lhs._33 -
				lhs._20  * lhs._03 * lhs._31 -
				lhs._30 * lhs._01 * lhs._23 +
				lhs._30 * lhs._03 * lhs._21;

			inv._31 = lhs._00  * lhs._21 * lhs._32 -
				lhs._00  * lhs._22 * lhs._31 -
				lhs._20  * lhs._01 * lhs._32 +
				lhs._20  * lhs._02 * lhs._31 +
				lhs._30 * lhs._01 * lhs._22 -
				lhs._30 * lhs._02 * lhs._21;

			inv._02 = lhs._01  * lhs._12 * lhs._33 -
				lhs._01  * lhs._13 * lhs._32 -
				lhs._11  * lhs._02 * lhs._33 +
				lhs._11  * lhs._03 * lhs._32 +
				lhs._31 * lhs._02 * lhs._13 -
				lhs._31 * lhs._03 * lhs._12;

			inv._12 = -lhs._00  * lhs._12 * lhs._33 +
				lhs._00  * lhs._13 * lhs._32 +
				lhs._10  * lhs._02 * lhs._33 -
				lhs._10  * lhs._03 * lhs._32 -
				lhs._30 * lhs._02 * lhs._13 +
				lhs._30 * lhs._03 * lhs._12;

			inv._22 = lhs._00  * lhs._11 * lhs._33 -
				lhs._00  * lhs._13 * lhs._31 -
				lhs._10  * lhs._01 * lhs._33 +
				lhs._10  * lhs._03 * lhs._31 +
				lhs._30 * lhs._01 * lhs._13 -
				lhs._30 * lhs._03 * lhs._11;

			inv._32 = -lhs._00  * lhs._11 * lhs._32 +
				lhs._00  * lhs._12 * lhs._31 +
				lhs._10  * lhs._01 * lhs._32 -
				lhs._10  * lhs._02 * lhs._31 -
				lhs._30 * lhs._01 * lhs._12 +
				lhs._30 * lhs._02 * lhs._11;

			inv._03 = -lhs._01 * lhs._12 * lhs._23 +
				lhs._01 * lhs._13 * lhs._22 +
				lhs._11 * lhs._02 * lhs._23 -
				lhs._11 * lhs._03 * lhs._22 -
				lhs._21 * lhs._02 * lhs._13 +
				lhs._21 * lhs._03 * lhs._12;

			inv._13 = lhs._00 * lhs._12 * lhs._23 -
				lhs._00 * lhs._13 * lhs._22 -
				lhs._10 * lhs._02 * lhs._23 +
				lhs._10 * lhs._03 * lhs._22 +
				lhs._20 * lhs._02 * lhs._13 -
				lhs._20 * lhs._03 * lhs._12;

			inv._23 = -lhs._00 * lhs._11 * lhs._23 +
				lhs._00 * lhs._13 * lhs._21 +
				lhs._10 * lhs._01 * lhs._23 -
				lhs._10 * lhs._03 * lhs._21 -
				lhs._20 * lhs._01 * lhs._13 +
				lhs._20 * lhs._03 * lhs._11;

			inv._33 = lhs._00 * lhs._11 * lhs._22 -
				lhs._00 * lhs._12 * lhs._21 -
				lhs._10 * lhs._01 * lhs._22 +
				lhs._10 * lhs._02 * lhs._21 +
				lhs._20 * lhs._01 * lhs._12 -
				lhs._20 * lhs._02 * lhs._11;

			float det = lhs._00 * inv._00 + lhs._01 * inv._10 + lhs._02 * inv._20 + lhs._03 * inv._30;
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