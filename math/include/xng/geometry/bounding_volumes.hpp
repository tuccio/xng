#pragma once

#include <xng/geometry/sphere.hpp>

namespace xng
{
	namespace math
	{
		/* bounding_sphere on iterable points */

		template <typename Iterator>
		sphere bounding_sphere(Iterator begin, Iterator end)
		{
			size_t size = std::distance(begin, end);

			if (!size)
			{
				return sphere(float3(0.f), 0.f);
			}

			float3 centroid(0.f);

			for (auto it = begin; it != end; it++)
			{
				centroid = centroid + to_float3(*it);
			}

			centroid = centroid / (float)size;

			float distance = 0.f;

			for (auto it = begin; it != end; ++it)
			{
				distance = max(length(centroid - to_float3(*it)), distance);
			}

			return sphere(centroid, distance);
		}

		/* bounding_aabb on iterable points */

		template <typename Iterator>
		aabb bounding_aabb(Iterator begin, Iterator end)
		{
			if (begin == end)
			{
				return aabb::from_center_half_extents(float3(0.f), float3(-FLT_MAX));
			}

			float3 min = to_float3(*begin);
			float3 max = to_float3(*begin);

			for (auto it = begin; it != end; it++)
			{
				float3 p = to_float3(*it);
				min = min(min, p);
				max = max(max, p);
			}

			return aabb::from_min_max(min, max);
		}

		/* bounding_aabb on sphere */

		XNG_INLINE aabb bounding_aabb(const sphere & s)
		{
			return aabb::from_center_half_extents(s.center, s.center);
		}
	}
}