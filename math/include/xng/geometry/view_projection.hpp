#pragma once

#include <xng/math.hpp>

namespace xng
{
    namespace math
    {
        float4x4 look_at_lh(const float3 & position, const float3 & target, const float3 & up);
        float4x4 look_at_rh(const float3 & position, const float3 & target, const float3 & up);

        float4x4 ortho_lh_directx(float l, float r, float b, float t, float n, float f);
        float4x4 ortho_rh_directx(float l, float r, float b, float t, float n, float f);

        float4x4 ortho_lh_gl(float l, float r, float b, float t, float n, float f);
        float4x4 ortho_rh_gl(float l, float r, float b, float t, float n, float f);
    }
}