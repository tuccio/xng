#include <xng/geometry/plane.hpp>

using namespace xng::math;

plane::plane(float a, float b, float c, float d) :
	v(a, b, c, d) {}

plane::plane(const float3 & normal, const float3 & point) :
	v(normal, -dot(normal, point)) {}

plane::plane(const float4 & plane) :
	v(plane) {}