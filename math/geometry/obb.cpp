#include <xng/geometry/obb.hpp>
#include <xng/geometry/aabb.hpp>

namespace xng
{
    namespace math
    {
        std::array<float3, 8> obb::get_corners(void) const
        {
            std::array<float3, 8> v;

            float3x3 m = transpose(reinterpret_cast<const float3x3&>(axes));

            v[XNG_OBB_FRONT_BOTTOM_RIGHT] = center + m * float3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
            v[XNG_OBB_FRONT_BOTTOM_LEFT]  = center + m * float3(halfExtents.x, -halfExtents.y, -halfExtents.z);
            v[XNG_OBB_FRONT_TOP_RIGHT]    = center + m * float3(-halfExtents.x, halfExtents.y, -halfExtents.z);
            v[XNG_OBB_FRONT_TOP_LEFT]     = center + m * float3(halfExtents.x, halfExtents.y, -halfExtents.z);
            v[XNG_OBB_BACK_BOTTOM_RIGHT]  = center + m * float3(-halfExtents.x, -halfExtents.y, halfExtents.z);
            v[XNG_OBB_BACK_BOTTOM_LEFT]   = center + m * float3(halfExtents.x, -halfExtents.y, halfExtents.z);
            v[XNG_OBB_BACK_TOP_RIGHT]     = center + m * float3(-halfExtents.x, halfExtents.y, halfExtents.z);
            v[XNG_OBB_BACK_TOP_LEFT]      = center + m * float3(halfExtents.x, halfExtents.y, halfExtents.z);

            return v;
        }

        obb to_obb(const aabb & aabb)
        {
            obb r;
            r.center = aabb.center;
            r.axes[0] = float3(1, 0, 0);
            r.axes[1] = float3(0, 1, 0);
            r.axes[2] = float3(0, 0, 1);
            r.halfExtents = aabb.halfExtents;
            return r;
        }

        obb transform(const obb & bb, const quaternion & q)
        {
            obb r;
            r.center = transform(bb.center, q);
            r.axes[0] = transform(bb.axes[0], q);
            r.axes[1] = transform(bb.axes[1], q);
            r.axes[2] = transform(bb.axes[2], q);
            r.halfExtents = bb.halfExtents;
            return r;
        }
    }
}
