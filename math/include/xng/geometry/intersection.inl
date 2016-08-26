#include <xng/math.hpp>

#include <xng/geometry/aabb.hpp>
#include <xng/geometry/sphere.hpp>

#include <algorithm>

namespace xng
{
	namespace math
	{
		namespace detail
		{

			/* aabb/aabb */

			template <>
			struct intersection_impl<aabb, aabb> :
				intersection_base<aabb, aabb>
			{

				XNG_INLINE static bool intersect(const aabb & a, const aabb & b)
				{
					float3 aCenter      = a.center;
					float3 aHalfExtents = a.halfExtents;

					float3 bCenter      = b.center;
					float3 bHalfExtents = b.halfExtents;

					float3 t1 = aCenter - bCenter;
					float3 t2 = aHalfExtents + bHalfExtents;

					return t2.x > t1.x && t2.y > t1.y && t2.z > t1.z;
				}

			};

			/* aabb/sphere */

			template <>
			struct intersection_impl<aabb, sphere> :
				intersection_base<aabb, sphere>
			{

				XNG_INLINE static bool intersect(const aabb & a, const sphere & b)
				{
					float3 aMin = a.get_min();
					float3 aMax = a.get_max();

					float3 closestPoint = {
						max(aMin.x, min(b.center.x, aMax.x)),
						max(aMin.y, min(b.center.y, aMax.y)),
						max(aMin.z, min(b.center.z, aMax.z))
					};

					if (closestPoint >= aMin && closestPoint <= aMax)
					{
						return true;
					}

					float3 e = closestPoint - b.center;
					return dot(e, e) < b.radius * b.radius;
				}

			};

			/* aabb/frustum */

			template <>
			struct intersection_impl<aabb, frustum> :
				intersection_base<aabb, frustum>
			{

				XNG_INLINE static bool intersect(aabb a, frustum b)
				{
					float3 min = a.get_min();
					float3 max = a.get_max();

					for (int i = 0; i < 6; i++)
					{

						float4 planeVector = b.planes[i].v;

						float3 normal = to_float3(planeVector);
						float3 n, p;

						if (normal.x > 0)
						{
							n.x = min.x;
							p.x = max.x;
						}
						else
						{
							n.x = max.x;
							p.x = min.x;
						}

						if (normal.y > 0)
						{
							n.y = min.y;
							p.y = max.y;
						}
						else
						{
							n.y = max.y;
							p.y = min.y;
						}

						if (normal.z > 0)
						{
							n.z = min.z;
							p.z = max.z;
						}
						else
						{
							n.z = max.z;
							p.z = min.z;
						}

						if (dot(plane(planeVector), n) > 0)
						{
							return false;
						}

					}

					return true;
				}

			};

			/* sphere/frustum */

			template <>
			struct intersection_impl<sphere, frustum> :
				intersection_base<sphere, frustum>
			{

				XNG_INLINE static bool intersect(const sphere & a, const frustum & b)
				{
					// http://www.flipcode.com/archives/Frustum_Culling.shtml

					for (int i = 0; i < 6; i++)
					{
						float distance = dot(b.planes[i], a.center);

						if (distance > a.radius)
						{
							return false;
						}
					}

					return true;
				}

			};

			/* ray/aabb */

			template <>
			struct intersection_impl<ray, aabb> :
				intersection_base<ray, aabb>
			{

				XNG_INLINE static bool intersect(const ray & a, const aabb & b, float & distance)
				{
					float3 invDirection  = 1.f / a.direction;
					float3 oInvDirection = a.origin * invDirection;

					float3 aabbMin = b.get_min();
					float3 aabbMax = b.get_max();

					float3 s0 = aabbMin - oInvDirection;
					float3 s1 = aabbMax - oInvDirection;

					float t1 = s0.m[0];
					float t2 = s1.m[0];
					float t3 = s0.m[1];
					float t4 = s1.m[1];
					float t5 = s0.m[2];
					float t6 = s1.m[2];

					float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
					float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

					if (tmax < 0)
					{
						distance = tmax;
						return false;
					}

					if (tmin > tmax)
					{
						distance = tmax;
						return false;
					}

					distance = tmin;
					return true;
				}

				XNG_INLINE static bool intersect(const ray & a, const aabb & b)
				{
					float3 aabbMin = b.get_min();
					float3 aabbMax = b.get_max();

					float3 origin = a.origin;

					float3 invDirection = 1.f / a.direction;

					float3 t1 = (aabbMin - origin) * invDirection;
					float3 t2 = (aabbMax - origin) * invDirection;

					float tmin = max(min(t1.x, t2.x), min(t1.y, t2.y));
					float tmax = min(max(t1.x, t2.x), max(t1.y, t2.y));

					return tmax >= tmin;
				}

			};

			/* ray/aabb */

			template <>
			struct intersection_impl<ray, sphere> :
				intersection_base<ray, sphere>
			{

				XNG_INLINE static bool intersect(const ray & a, const sphere & b, float & distance)
				{
					float3 oc = a.origin - b.center;

					// Assuming the ray direction is a normalized vector, so A = 1
					float B = 2 * dot(a.direction, oc);
					float C = dot(oc, oc) - b.radius * b.radius;

					float delta = B * B - 4 * C;

					if (delta >= 0.f)
					{
						float sqrtDelta = square_root(delta);
						float t0 = (-B - sqrtDelta) * .5f;

						if (t0 >= 0.f)
						{
							distance = t0;
							return true;
						}
						else
						{
							float t1 = (-B + sqrtDelta) * .5f;

							if (t1 >= 0.f)
							{
								distance = t1;
								return true;
							}
						}
					}
					return false;
				}

				XNG_INLINE static bool intersect(const ray & a, const sphere & b)
				{
					float t;
					return intersect(a, b, t);
				}

			};

		}
	}
}