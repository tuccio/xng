#pragma once

namespace xng
{
	namespace math
	{
		XNG_INLINE bool operator< (const float4 & a, const float4 & b)
		{
			return a.x < b.x && a.y < b.y && a.z < b.z && a.w < b.w;
		}

		XNG_INLINE bool operator> (const float4 & a, const float4 & b)
		{
			return a.x > b.x && a.y > b.y && a.z > b.z && a.w > b.w;
		}

		XNG_INLINE bool operator<= (const float4 & a, const float4 & b)
		{
			return a.x <= b.x && a.y <= b.y && a.z <= b.z && a.w <= b.w;
		}

		XNG_INLINE bool operator>= (const float4 & a, const float4 & b)
		{
			return a.x >= b.x && a.y >= b.y && a.z >= b.z && a.w >= b.w;
		}
	}
}