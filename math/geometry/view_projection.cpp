#pragma once

#include <xng/geometry/view_projection.hpp>

namespace xng
{
	namespace math
	{
		float4x4 look_at_lh(const float3 & position, const float3 & target, const float3 & up)
		{
			float3 z = normalize(target - position);
			float3 x = normalize(cross(up, z));
			float3 y = cross(z, x);

			return float4x4(
				x.x, x.y, x.z, -dot(x, position),
				y.x, y.y, y.z, -dot(y, position),
				z.x, z.y, z.z, -dot(z, position),
				0.f, 0.f, 0.f, 1.f
			);
		}

		float4x4 look_at_rh(const float3 & position, const float3 & target, const float3 & up)
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
	}
}