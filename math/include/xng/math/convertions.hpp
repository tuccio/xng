#pragma once

#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix3x3.hpp"
#include "matrix4x4.hpp"

namespace xng
{
	namespace math
	{

		/* to_float3 */

		XNG_INLINE float3 to_float3(const float3 & lhs)
		{
			return lhs;
		}

		XNG_INLINE float3 to_float3(const float4 & lhs)
		{
			return float3(lhs.x, lhs.y, lhs.z);
		}

		XNG_INLINE float3 to_float3(const float3row & lhs)
		{
			return float3(lhs.m[0], lhs.m[1], lhs.m[2]);
		}

		XNG_INLINE float3 to_float3(const float4row & lhs)
		{
			return float3(lhs.m[0], lhs.m[1], lhs.m[2]);
		}

		/* to_float4 */

		XNG_INLINE float4 to_float4(const float3 & lhs)
		{
			return float4(lhs.x, lhs.y, lhs.z, 1.f);
		}

		XNG_INLINE float4 to_float4(const float4 & lhs)
		{
			return lhs;
		}

		XNG_INLINE float4 to_float4(const float3row & lhs)
		{
			return float4(lhs.m[0], lhs.m[1], lhs.m[2], 1.f);
		}

		XNG_INLINE float4 to_float4(const float4row & lhs)
		{
			return float4(lhs.m[0], lhs.m[1], lhs.m[2], lhs.m[3]);
		}

		/* to_float3x3 */

		XNG_INLINE float3x3 to_float3x3(const float4x4 & lhs)
		{
			return float3x3(
				lhs._00, lhs._01, lhs._02,
				lhs._10, lhs._11, lhs._12,
				lhs._20, lhs._21, lhs._22);
		}

		/* to_float4x4 */

		XNG_INLINE float4x4 to_float4x4(const float3x3 & lhs)
		{
			return float4x4(
				lhs._00, lhs._01, lhs._02, 0.f,
				lhs._10, lhs._11, lhs._12, 0.f,
				lhs._20, lhs._21, lhs._22, 0.f,
				0.f, 0.f, 0.f, 1.f);
		}

	}
}