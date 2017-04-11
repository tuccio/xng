#pragma once

#include <xng/geometry/plane.hpp>
#include <array>

#define XNG_MAKE_FRUSTUM_CORNER(Z, Y, X) ((X) | (Y << 1) | (Z << 2))

enum xng_frustum_corners
{
    XNG_FRUSTUM_NEAR_BOTTOM_RIGHT = XNG_MAKE_FRUSTUM_CORNER(0, 0, 0),
    XNG_FRUSTUM_NEAR_BOTTOM_LEFT  = XNG_MAKE_FRUSTUM_CORNER(0, 0, 1),
    XNG_FRUSTUM_NEAR_TOP_RIGHT    = XNG_MAKE_FRUSTUM_CORNER(0, 1, 0),
    XNG_FRUSTUM_NEAR_TOP_LEFT     = XNG_MAKE_FRUSTUM_CORNER(0, 1, 1),
    XNG_FRUSTUM_FAR_BOTTOM_RIGHT  = XNG_MAKE_FRUSTUM_CORNER(1, 0, 0),
    XNG_FRUSTUM_FAR_BOTTOM_LEFT   = XNG_MAKE_FRUSTUM_CORNER(1, 0, 1),
    XNG_FRUSTUM_FAR_TOP_RIGHT     = XNG_MAKE_FRUSTUM_CORNER(1, 1, 0),
    XNG_FRUSTUM_FAR_TOP_LEFT      = XNG_MAKE_FRUSTUM_CORNER(1, 1, 1)
};

enum xng_frustum_planes
{
    XNG_FRUSTUM_PLANE_NEAR,
    XNG_FRUSTUM_PLANE_FAR,
    XNG_FRUSTUM_PLANE_TOP,
    XNG_FRUSTUM_PLANE_BOTTOM,
    XNG_FRUSTUM_PLANE_LEFT,
    XNG_FRUSTUM_PLANE_RIGHT
};

enum xng_coordinate_system
{
    XNG_COORDINATE_SYSTEM_OPENGL,
    XNG_COORDINATE_SYSTEM_DIRECTX
};

namespace xng
{
    namespace math
    {
        struct frustum
        {
            frustum(void) = default;
            frustum(const frustum &) = default;
            frustum(frustum &&) = default;

            frustum(const float4x4 & viewProjection, xng_coordinate_system cs);

            frustum & operator= (const frustum &) = default;
            frustum & operator= (frustum &&) = default;

            std::array<float3, 8> get_corners(void) const;

            std::array<plane, 6> planes;
        };
    }
}