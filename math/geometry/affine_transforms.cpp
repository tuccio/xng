#pragma once

#include <xng/math.hpp>

namespace xng
{
    namespace math
    {
        void decompose_affine(const float4x4 & m, float3 * pScale, quaternion * pRotation, float3 * pTranslation)
        {
            float3 scale, translation;
            quaternion rotation;

            // Extract translation

            translation = float3(m._03, m._13, m._23);

            // Extract scale

            float3x3 SR = to_float3x3(m);

            scale.x = length(SR.r[0]);
            scale.y = length(SR.r[1]);
            scale.z = length(SR.r[2]);

            float det = determinant(SR);

            if (det < .0f)
            {
                scale.x = -scale.x;
            }

            // Extract rotation

            float3x3 R;

            R.r[0] = SR.r[0] / scale.x;
            R.r[1] = SR.r[1] / scale.y;
            R.r[2] = SR.r[2] / scale.z;

            rotation = to_quaternion(R);

            // Output

            *pScale       = scale;
            *pRotation    = rotation;
            *pTranslation = translation;
        }
    }    
}