#pragma once

#include <xng/math.hpp>

namespace xng
{
    namespace math
    {
        struct ray
        {
            ray(const float3 & origin, const float3 & direction) :
                origin(origin), direction(direction) {}

            ray(void) = default;
            ray(const ray &) = default;
            ray(ray &&) = default;

            ray & operator= (const ray &) = default;
            ray & operator= (ray &&) = default;

            float3 origin;
            float3 direction;
        };

        XNG_INLINE float3 evaluate(const ray & r, float t)
        {
            return r.origin + (t * r.direction);
        }
    }
}