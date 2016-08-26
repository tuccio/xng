#pragma once

#include <xng/geometry/sphere.hpp>

namespace xng
{
	namespace math
	{
		/*XNG_INLINE sphere transform_affine(const sphere & s, const mat128 & transform)
		{
			vec128 svec = s.get_sphere_vector();

			vec128 newCenter = mat128_transform3(svec, transform);
			vec128 surfacePoint = vec128_permute<XNG_W1, XNG_Y0, XNG_Z0, XNG_W1>(vec128_zero(), svec);

			surfacePoint = mat128_transform3(svec + surfacePoint, transform);

			vec128 distance = vec128_length3(surfacePoint - newCenter);

			return sphere(vec128_select<XNG_X0, XNG_Y0, XNG_Z0, XNG_W1>(newCenter, distance));
		}*/
	}
}

