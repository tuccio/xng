#include "scalar_functions.hpp"

#include <cmath>
#include <algorithm>

namespace xng
{
	namespace math
	{
		/* Arithmetic */

		XNG_INLINE quaternion operator+ (const quaternion & lhs, const quaternion & rhs)
		{
			return quaternion(
				lhs.w + rhs.w,
				lhs.w + rhs.x,
				lhs.w + rhs.y,
				lhs.w + rhs.z);
		}

		XNG_INLINE quaternion operator- (const quaternion & lhs, const quaternion & rhs)
		{
			return quaternion(
				lhs.w - rhs.w,
				lhs.w - rhs.x,
				lhs.w - rhs.y,
				lhs.w - rhs.z);
		}

		XNG_INLINE quaternion operator* (const quaternion & lhs, const quaternion & rhs)
		{
			return quaternion(
				lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
				lhs.x * rhs.w + lhs.w * rhs.x + lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
				lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w);
		}

		XNG_INLINE quaternion conjugate(const quaternion & q)
		{
			return quaternion(q.w, -q.x, -q.y, -q.z);
		}

		XNG_INLINE quaternion inverse(const quaternion & q)
		{
			float invSquaredNorm = 1.f / (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
			return quaternion(q.w * invSquaredNorm, -q.x * invSquaredNorm, -q.y * invSquaredNorm, -q.z * invSquaredNorm);
		}

		XNG_INLINE float norm(const quaternion & q)
		{
			return square_root(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
		}

		XNG_INLINE quaternion normalize(const quaternion & q)
		{
			float invNorm = inverse_square_root(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
			return quaternion(q.w * invNorm, q.x * invNorm, q.y * invNorm, q.z * invNorm);
		}

		XNG_INLINE quaternion slerp(const quaternion & a, const quaternion & b, float t)
		{
			float cosHalfTheta = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

			if (abs(cosHalfTheta) >= 1.f)
			{
				return a;
			}

			float halfTheta    = std::acos(cosHalfTheta);
			float sinHalfTheta = square_root(1.f - cosHalfTheta*cosHalfTheta);

			if (fabs(sinHalfTheta) < 0.001f)
			{
				return quaternion(a.w * .5f + b.w * .5f,
					a.x * .5f + b.x * .5f,
					a.y * .5f + b.y * .5f,
					a.z * .5f + b.z * .5f);
			}

			float ratioA = std::sin((1 - t) * halfTheta) / sinHalfTheta;
			float ratioB = std::sin(t * halfTheta) / sinHalfTheta;

			return quaternion(a.w * ratioA + b.w * ratioB,
				a.x * ratioA + b.x * ratioB,
				a.y * ratioA + b.y * ratioB,
				a.z * ratioA + b.z * ratioB);
		}

		XNG_INLINE float3 transform(const float3 & lhs, const quaternion & rhs)
		{
			float3 u(rhs.x, rhs.y, rhs.z);
			float3 uv = 2.f * cross(u, lhs);
			return lhs + rhs.w * uv + cross(u, uv);
		}

		XNG_INLINE float4 transform(const float4 & lhs, const quaternion & rhs)
		{
			float3 r = transform(reinterpret_cast<const float3 &>(lhs), rhs);
			return float4(r.x, r.y, r.z, 1.f);
		}

		XNG_INLINE float3 operator* (const quaternion & lhs, const float3 & rhs)
		{
			return transform(rhs, lhs);
		}

		XNG_INLINE float4 operator* (const quaternion & lhs, const float4 & rhs)
		{
			return transform(rhs, lhs);
		}

		XNG_INLINE quaternion to_quaternion(const float3 & eulerAngles)
		{
			float3 h = eulerAngles * .5f;
			float c1 = std::cos(h.y);
			float s1 = std::sin(h.y);
			float c2 = std::cos(h.x);
			float s2 = std::sin(h.x);
			float c3 = std::cos(h.z);
			float s3 = std::sin(h.z);
			quaternion q;
			q.w = c1 * c2 * c3 - s1 * s2 * s3;
			q.x = s1 * s2 * c3 + c1 * c2 * s3;
			q.y = s1 * c2 * c3 + c1 * s2 * s3;
			q.z = c1 * s2 * c3 - s1 * c2 * s3;
			return q;
		}

		XNG_INLINE float3 to_euler(const quaternion & q)
		{
			float _2zz = 2 * q.z * q.z;
			float y = std::atan2(2.f * q.y * q.w - 2.f * q.x * q.z, 1.f - 2.f * q.y * q.y - _2zz);
			float x = std::asin(2.f * q.x * q.y + 2 * q.z * q.w);
			float z = std::atan2(2.f * q.x * q.w - 2 * q.y * q.z, 1 - 2 * q.x * q.x - _2zz);
			return float3(x, y, z);
		}

		XNG_INLINE quaternion to_quaternion(const float3x3 & lhs)
		{
			quaternion q;
			q.w = square_root(max(0.f, 1.f + lhs._00 + lhs._11 + lhs._22)) * .5f;
			q.x = square_root(max(0.f, 1.f + lhs._00 - lhs._11 - lhs._22)) * .5f;
			q.y = square_root(max(0.f, 1.f - lhs._00 + lhs._11 - lhs._22)) * .5f;
			q.z = square_root(max(0.f, 1.f - lhs._00 - lhs._11 + lhs._22)) * .5f;
			q.x = std::copysign(q.x, lhs._21 - lhs._12);
			q.y = std::copysign(q.y, lhs._02 - lhs._20);
			q.z = std::copysign(q.z, lhs._10 - lhs._01);
			return q;
		}

		XNG_INLINE quaternion to_quaternion(const float4x4 & lhs)
		{
			quaternion q;
			q.w = square_root(max(0.f, 1.f + lhs._00 + lhs._11 + lhs._22)) * .5f;
			q.x = square_root(max(0.f, 1.f + lhs._00 - lhs._11 - lhs._22)) * .5f;
			q.y = square_root(max(0.f, 1.f - lhs._00 + lhs._11 - lhs._22)) * .5f;
			q.z = square_root(max(0.f, 1.f - lhs._00 - lhs._11 + lhs._22)) * .5f;
			q.x = std::copysign(q.x, lhs._21 - lhs._12);
			q.y = std::copysign(q.y, lhs._02 - lhs._20);
			q.z = std::copysign(q.z, lhs._10 - lhs._01);
			return q;
		}
	}
}