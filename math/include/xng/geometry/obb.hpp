#pragma once

#include <xng/math.hpp>
#include <xng/geometry/forward.hpp>

#define XNG_MAKE_OBB_CORNER(Z, Y, X) ((X) | (Y << 1) | (Z << 2))

enum obb_corners
{
    XNG_OBB_FRONT_BOTTOM_RIGHT = XNG_MAKE_OBB_CORNER(0, 0, 0),
    XNG_OBB_FRONT_BOTTOM_LEFT  = XNG_MAKE_OBB_CORNER(0, 0, 1),
    XNG_OBB_FRONT_TOP_RIGHT    = XNG_MAKE_OBB_CORNER(0, 1, 0),
    XNG_OBB_FRONT_TOP_LEFT     = XNG_MAKE_OBB_CORNER(0, 1, 1),
    XNG_OBB_BACK_BOTTOM_RIGHT  = XNG_MAKE_OBB_CORNER(1, 0, 0),
    XNG_OBB_BACK_BOTTOM_LEFT   = XNG_MAKE_OBB_CORNER(1, 0, 1),
    XNG_OBB_BACK_TOP_RIGHT     = XNG_MAKE_OBB_CORNER(1, 1, 0),
    XNG_OBB_BACK_TOP_LEFT      = XNG_MAKE_OBB_CORNER(1, 1, 1)
};

namespace xng
{
    namespace math
    {
        struct obb
        {

        public:

            obb(void) = default;
            obb(const obb &) = default;
            obb(obb &&) = default;

            XNG_INLINE obb & operator= (const obb &) = default;
            XNG_INLINE obb & operator= (obb &&) = default;

            std::array<float3, 8> get_corners(void) const;

        public:

            float3 center;
            float3 axes[3];
            float3 halfExtents;

        };

        obb to_obb(const aabb & aabb);

        obb transform(const obb & bb, const quaternion & q);
    }
}