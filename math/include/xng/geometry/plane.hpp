#pragma once

#include <xng/math.hpp>

namespace xng
{
    namespace math
    {
        struct plane
        {
            plane(void) = default;
            plane(const plane &) = default;
            plane(plane &&) = default;

            plane(float a, float b, float c, float d);
            plane(const float3 & normal, const float3 & point);
            plane(const float4 & plane);

            plane & operator= (const plane &) = default;
            plane & operator= (plane &&) = default;

            union
            {
                float4 v;
                struct
                {
                    float a, b, c, d;
                };
            };
        };

        XNG_INLINE plane flip(const plane & p)
        {
            return plane(-p.v);
        }

        XNG_INLINE plane normalize(const plane & p)
        {
            float3 N = to_float3(p.v);
            return plane(inverse_square_root(dot(N, N)) * p.v);
        }

        XNG_INLINE float dot(const plane & lhs, const float3 & rhs)
        {
            return dot(to_float4(rhs), lhs.v);
        }

    }
}