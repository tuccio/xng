#pragma once

namespace xng
{
	namespace math
	{
		XNG_INLINE float3 cross(const float3 & lhs, const float3 & rhs)
		{
			return float3(
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x);
		}

		XNG_INLINE matrix<float, 1, 3> cross(const matrix<float, 1, 3> & lhs, const matrix<float, 1, 3> & rhs)
		{
			float3 r = cross(reinterpret_cast<const float3&>(lhs), reinterpret_cast<const float3&>(rhs));
			return transpose(r);
		}

		XNG_INLINE bool operator< (const float3 & a, const float3 & b)
		{
			return a.x < b.x && a.y < b.y && a.z < b.z;
		}

		XNG_INLINE bool operator> (const float3 & a, const float3 & b)
		{
			return a.x > b.x && a.y > b.y && a.z > b.z;
		}

		XNG_INLINE bool operator<= (const float3 & a, const float3 & b)
		{
			return a.x <= b.x && a.y <= b.y && a.z <= b.z;
		}

		XNG_INLINE bool operator>= (const float3 & a, const float3 & b)
		{
			return a.x >= b.x && a.y >= b.y && a.z >= b.z;
		}

	}
}