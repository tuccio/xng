#pragma once

namespace xng
{
    namespace math
    {

        XNG_INLINE float3x3 to_scale3(const float3 & lhs)
        {
            return float3x3(
                lhs.x, 0.f, 0.f,
                0.f, lhs.y, 0.f,
                0.f, 0.f, lhs.z
            );
        }

        XNG_INLINE float4x4 to_scale4(const float3 & lhs)
        {
            return float4x4(
                lhs.x, 0.f, 0.f, 0.f,
                0.f, lhs.y, 0.f, 0.f,
                0.f, 0.f, lhs.z, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        XNG_INLINE float4x4 to_translation4(const float3 & lhs)
        {
            return float4x4(
                1.f, 0.f, 0.f, lhs.x,
                0.f, 1.f, 0.f, lhs.y,
                0.f, 0.f, 1.f, lhs.z,
                0.f, 0.f, 0.f, 1.f
            );
        }

        XNG_INLINE float3x3 to_rotation3(const quaternion & lhs)
        {
            float3x3 m;

            float xx = lhs.x * lhs.x;
            float xy = lhs.x * lhs.y;
            float xz = lhs.x * lhs.z;
            float xw = lhs.x * lhs.w;

            float yy = lhs.y * lhs.y;
            float yz = lhs.y * lhs.z;
            float yw = lhs.y * lhs.w;

            float zz = lhs.z * lhs.z;
            float zw = lhs.z * lhs.w;

            m._00 = 1 - 2 * (yy + zz);
            m._01 = 2 * (xy - zw);
            m._02 = 2 * (xz + yw);

            m._10 = 2 * (xy + zw);
            m._11 = 1 - 2 * (xx + zz);
            m._12 = 2 * (yz - xw);

            m._20 = 2 * (xz - yw);
            m._21 = 2 * (yz + xw);
            m._22 = 1 - 2 * (xx + yy);

            return m;
        }

        XNG_INLINE float4x4 to_rotation4(const quaternion & lhs)
        {
            float4x4 m;

            float xx = lhs.x * lhs.x;
            float xy = lhs.x * lhs.y;
            float xz = lhs.x * lhs.z;
            float xw = lhs.x * lhs.w;

            float yy = lhs.y * lhs.y;
            float yz = lhs.y * lhs.z;
            float yw = lhs.y * lhs.w;

            float zz = lhs.z * lhs.z;
            float zw = lhs.z * lhs.w;

            m._00 = 1 - 2 * (yy + zz);
            m._01 = 2 * (xy - zw);
            m._02 = 2 * (xz + yw);

            m._10 = 2 * (xy + zw);
            m._11 = 1 - 2 * (xx + zz);
            m._12 = 2 * (yz - xw);

            m._20 = 2 * (xz - yw);
            m._21 = 2 * (yz + xw);
            m._22 = 1 - 2 * (xx + yy);

            m._03 = m._13 = m._23 = m._30 = m._31 = m._32 = 0;
            m._33 = 1;

            return m;
        }

        XNG_INLINE float3 to_euler(const float3x3 & m)
        {
            float3 e;
            e.x = std::asin(m._10);
            e.y = std::atan2(-m._20, m._00);
            e.z = std::atan2(-m._12, m._11);            
            return e;
        }
        
        void decompose_affine(const float4x4 & m, float3 * pScale, quaternion * pRotation, float3 * pTranslation);

    }
    
}