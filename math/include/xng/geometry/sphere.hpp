#pragma once

#include <xng/core.hpp>
#include <xng/math.hpp>

#include <type_traits>

namespace xng
{
	namespace math
	{
		struct sphere
		{
			sphere(void) = default;
			sphere(const sphere &) = default;
			sphere(sphere &&) = default;

			sphere(const float3 & center, float radius) :
				center(center), radius(radius) {}

			sphere & operator= (const sphere &) = default;
			sphere & operator= (sphere &&) = default;

			float3 center;
			float  radius;
		};
	}
}