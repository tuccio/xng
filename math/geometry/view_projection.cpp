#pragma once

#include <xng/geometry/view_projection.hpp>

namespace xng
{
	namespace math
	{
		float4x4 look_at_lh(const float3 & position, const float3 & up, const float3 & target)
		{
			float3 z = normalize(target - position);
			float3 x = normalize(cross(up, z));
			float3 y = cross(z, x);

			return float4x4(
				x.x, x.y, x.z, dot(x, position),
				y.x, y.y, y.z, dot(y, position),
				z.x, z.y, z.z, dot(z, position),
				0.f, 0.f, 0.f, 1.f
			);
		}

		float4x4 look_at_rh(const float3 & position, const float3 & up, const float3 & target)
		{
			float3 z = normalize(position - target);
			float3 x = normalize(cross(up, z));
			float3 y = cross(z, x);

			return float4x4(
				x.x, x.y, x.z, -dot(x, position),
				y.x, y.y, y.z, -dot(y, position),
				z.x, z.y, z.z, -dot(z, position),
				0.f, 0.f, 0.f, 1.f
			);
		}

		float4x4 ortho_lh_directx(float l, float r, float b, float t, float n, float f)
		{
			float invRmL = 1.f / (r - l);
			float invTmB = 1.f / (t - b);
			float invFmN = 1.f / (f - n);

			return float4x4(
				2.f / invRmL, 0, 0, -(l + r) * invRmL,
				0, 2.f / invTmB, 0, -(t + b) * invTmB,
				0, 0, 1.f * invFmN, -n * invFmN,
				0, 0, 0, 1
			);
		}

		float4x4 ortho_rh_directx(float l, float r, float b, float t, float n, float f)
		{
			float invRmL = 1.f / (r - l);
			float invTmB = 1.f / (t - b);
			float invNmF = 1.f / (f - n);

			return float4x4(
				2.f * invRmL, 0, 0, -(l + r) * invRmL,
				0, 2.f * invTmB, 0, -(t + b) * invTmB,
				0, 0, 1.f * invNmF, n * invNmF,
				0, 0, 0, 1
			);
		}

		/*float4x4 ortho_lh_gl(float l, float r, float b, float t, float n, float f)
		{

		}

		float4x4 ortho_rh_gl(float l, float r, float b, float t, float n, float f)
		{

		}*/
	}
}