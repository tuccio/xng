#include <xng/geometry/frustum.hpp>
#include <xng/geometry/cramer.hpp>

using namespace xng::math;

frustum::frustum(const float4x4 & viewProjection, xng_coordinate_system cs)
{
	if (cs == XNG_COORDINATE_SYSTEM_DIRECTX)
	{
		planes[XNG_FRUSTUM_PLANE_NEAR]   = flip(normalize(plane(transpose(viewProjection.r[2]))));
		planes[XNG_FRUSTUM_PLANE_FAR]    = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[2]))));
		planes[XNG_FRUSTUM_PLANE_TOP]    = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[1]))));
		planes[XNG_FRUSTUM_PLANE_BOTTOM] = flip(normalize(plane(transpose(viewProjection.r[3] + viewProjection.r[1]))));
		planes[XNG_FRUSTUM_PLANE_LEFT]   = flip(normalize(plane(transpose(viewProjection.r[3] + viewProjection.r[0]))));
		planes[XNG_FRUSTUM_PLANE_RIGHT]  = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[0]))));
	}
	else
	{
		// TODO
		planes[XNG_FRUSTUM_PLANE_NEAR]   = flip(normalize(plane(transpose(viewProjection.r[2]))));
		planes[XNG_FRUSTUM_PLANE_FAR]    = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[2]))));
		planes[XNG_FRUSTUM_PLANE_TOP]    = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[1]))));
		planes[XNG_FRUSTUM_PLANE_BOTTOM] = flip(normalize(plane(transpose(viewProjection.r[3] + viewProjection.r[1]))));
		planes[XNG_FRUSTUM_PLANE_LEFT]   = flip(normalize(plane(transpose(viewProjection.r[3] + viewProjection.r[0]))));
		planes[XNG_FRUSTUM_PLANE_RIGHT]  = flip(normalize(plane(transpose(viewProjection.r[3] - viewProjection.r[0]))));
	}
}

static XNG_INLINE float3 plane_intersection(const plane & p1, const plane & p2, const plane & p3)
{
	float3x3 A;
	float3 b;

	A.r[0] = transpose(to_float3(p1.v));
	A.r[1] = transpose(to_float3(p2.v));
	A.r[2] = transpose(to_float3(p3.v));

	b = float3(-p1.v.w, -p2.v.w, -p3.v.w);

	return cramer3(A, b);
}

std::array<float3, 8> frustum::get_corners(void) const
{
	std::array<float3, 8> corners;

	corners[XNG_FRUSTUM_NEAR_BOTTOM_RIGHT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_NEAR],
		planes[XNG_FRUSTUM_PLANE_BOTTOM],
		planes[XNG_FRUSTUM_PLANE_RIGHT]);

	corners[XNG_FRUSTUM_NEAR_BOTTOM_LEFT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_NEAR],
		planes[XNG_FRUSTUM_PLANE_BOTTOM],
		planes[XNG_FRUSTUM_PLANE_LEFT]);

	corners[XNG_FRUSTUM_NEAR_TOP_RIGHT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_NEAR],
		planes[XNG_FRUSTUM_PLANE_TOP],
		planes[XNG_FRUSTUM_PLANE_RIGHT]);

	corners[XNG_FRUSTUM_NEAR_TOP_LEFT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_NEAR],
		planes[XNG_FRUSTUM_PLANE_TOP],
		planes[XNG_FRUSTUM_PLANE_LEFT]);

	corners[XNG_FRUSTUM_FAR_BOTTOM_RIGHT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_FAR],
		planes[XNG_FRUSTUM_PLANE_BOTTOM],
		planes[XNG_FRUSTUM_PLANE_RIGHT]);

	corners[XNG_FRUSTUM_FAR_BOTTOM_LEFT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_FAR],
		planes[XNG_FRUSTUM_PLANE_BOTTOM],
		planes[XNG_FRUSTUM_PLANE_LEFT]);

	corners[XNG_FRUSTUM_FAR_TOP_RIGHT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_FAR],
		planes[XNG_FRUSTUM_PLANE_TOP],
		planes[XNG_FRUSTUM_PLANE_RIGHT]);

	corners[XNG_FRUSTUM_FAR_TOP_LEFT] = plane_intersection(
		planes[XNG_FRUSTUM_PLANE_FAR],
		planes[XNG_FRUSTUM_PLANE_TOP],
		planes[XNG_FRUSTUM_PLANE_LEFT]);

	return corners;
}