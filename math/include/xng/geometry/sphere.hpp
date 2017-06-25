#pragma once

#include <xng/math.hpp>

#include <type_traits>

namespace xng
{
    namespace math
    {
        struct sphere
        {
            sphere(void) = default;
            sphere(const sphere &) = default;
            sphere(sphere &&) = default;

            sphere(const float3 & center, float radius) :
                center(center), radius(radius) {}

            sphere & operator= (const sphere &) = default;
            sphere & operator= (sphere &&) = default;

            float3 center;
            float  radius;
        };

        inline sphere transform(const sphere & s, const float4x4 & m)
        {
            float3 newCenter   = to_float3(m * float4(s.center, 1.f));
            float3 newFarPoint = to_float3(m * float4(s.center.x + s.radius, s.center.y, s.center.z, 1.f));
            return sphere(newCenter, length(newFarPoint - newCenter));
        }
    }
}