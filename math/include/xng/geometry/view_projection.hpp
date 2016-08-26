#pragma once

#include <xng/math.hpp>

namespace xng
{
	namespace math
	{
		float4x4 look_at_lh(const float3 & position, const float3 & target, const float3 & up);
		float4x4 look_at_rh(const float3 & position, const float3 & target, const float3 & up);
	}
}