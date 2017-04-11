#pragma once

#include <xng/math.hpp>

#include <algorithm>
#include <array>

#define XNG_MAKE_AABB_CORNER(Z, Y, X) ((X) | (Y << 1) | (Z << 2))

enum aabb_corners
{
    XNG_AABB_FRONT_BOTTOM_RIGHT = XNG_MAKE_AABB_CORNER(0, 0, 0),
    XNG_AABB_FRONT_BOTTOM_LEFT  = XNG_MAKE_AABB_CORNER(0, 0, 1),
    XNG_AABB_FRONT_TOP_RIGHT    = XNG_MAKE_AABB_CORNER(0, 1, 0),
    XNG_AABB_FRONT_TOP_LEFT     = XNG_MAKE_AABB_CORNER(0, 1, 1),
    XNG_AABB_BACK_BOTTOM_RIGHT  = XNG_MAKE_AABB_CORNER(1, 0, 0),
    XNG_AABB_BACK_BOTTOM_LEFT   = XNG_MAKE_AABB_CORNER(1, 0, 1),
    XNG_AABB_BACK_TOP_RIGHT     = XNG_MAKE_AABB_CORNER(1, 1, 0),
    XNG_AABB_BACK_TOP_LEFT      = XNG_MAKE_AABB_CORNER(1, 1, 1)
};

namespace xng
{
    namespace math
    {
        struct aabb
        {

        public:

            aabb(void) = default;
            aabb(const aabb &) = default;
            aabb(aabb &&) = default;

            static XNG_INLINE aabb from_min_max(const float3 & min, const float3 & max)
            {
                aabb box;
                box.center      = (min + max) * .5f;
                box.halfExtents = (max - min) * .5f;
                return box;
            }

            static XNG_INLINE aabb from_center_half_extents(const float3 & center, const float3 & halfExtents)
            {
                aabb box;
                box.center      = center;
                box.halfExtents = halfExtents;
                return box;
            }

            XNG_INLINE aabb & operator= (const aabb &) = default;
            XNG_INLINE aabb & operator= (aabb &&) = default;

            XNG_INLINE float3 get_min(void) const { return center - halfExtents; }
            XNG_INLINE float3 get_max(void) const { return center + halfExtents; }

            std::array<float3, 8> get_corners(void) const;

            struct
            {
                float3 center;
                float3 halfExtents;
            };

        };

        aabb operator+ (const aabb & a, const aabb & b);
        aabb operator^ (const aabb & a, const aabb & b);

        template <typename Iterator>
        inline aabb make_aabb(Iterator begin, Iterator end)
        {
            auto mMx = std::minmax_element(begin, end, [](const float3 & lhs, const float3 & rhs) { return lhs.x < rhs.x; });
            auto mMy = std::minmax_element(begin, end, [](const float3 & lhs, const float3 & rhs) { return lhs.y < rhs.y; });
            auto mMz = std::minmax_element(begin, end, [](const float3 & lhs, const float3 & rhs) { return lhs.z < rhs.z; });

            return aabb::from_min_max(float3{ mMx.first->x, mMy.first->y, mMz.first->z },
                                      float3{ mMx.second->x, mMy.second->y, mMz.second->z });
        }
    }
}