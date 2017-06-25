#include <xng/geometry/aabb.hpp>

using namespace xng::math;

aabb xng::math::operator+ (const aabb & a, const aabb & b)
{
    bool aEmpty = a.empty();
    bool bEmpty = b.empty();

    return aabb::from_min_max(
        min(!aEmpty ? a.get_min() : b.get_min(), !bEmpty ? b.get_min() : a.get_min()),
        max(!aEmpty ? a.get_max() : b.get_max(), !bEmpty ? b.get_max() : a.get_max())
    );
}

aabb xng::math::operator^ (const aabb & a, const aabb & b)
{
    return aabb::from_min_max(
        max(a.get_min(), b.get_min()),
        min(a.get_max(), b.get_max())
    );
}

std::array<float3, 8> aabb::get_corners(void) const
{
    std::array<float3, 8> v;

    v[XNG_AABB_FRONT_BOTTOM_RIGHT] = center + float3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
    v[XNG_AABB_FRONT_BOTTOM_LEFT]  = center + float3(halfExtents.x, -halfExtents.y, -halfExtents.z);
    v[XNG_AABB_FRONT_TOP_RIGHT]    = center + float3(-halfExtents.x, halfExtents.y, -halfExtents.z);
    v[XNG_AABB_FRONT_TOP_LEFT]     = center + float3(halfExtents.x, halfExtents.y, -halfExtents.z);
    v[XNG_AABB_BACK_BOTTOM_RIGHT]  = center + float3(-halfExtents.x, -halfExtents.y, halfExtents.z);
    v[XNG_AABB_BACK_BOTTOM_LEFT]   = center + float3(halfExtents.x, -halfExtents.y, halfExtents.z);
    v[XNG_AABB_BACK_TOP_RIGHT]     = center + float3(-halfExtents.x, halfExtents.y, halfExtents.z);
    v[XNG_AABB_BACK_TOP_LEFT]      = center + float3(halfExtents.x, halfExtents.y, halfExtents.z);

    return v;
}

bool aabb::empty(void) const
{
    return halfExtents == float3::zero();
}