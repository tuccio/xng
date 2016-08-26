#pragma once

#include <cstdint>

#include <xng/math.hpp>
#include "leading_zeros.hpp"

namespace xng
{
	namespace math
	{
		typedef uint64_t morton_code;

		struct morton_unpacked
		{
			uint32_t x;
			uint32_t y;
			uint32_t z;
		};

#ifdef XNG_SSE

		namespace detail
		{

			union sse_register {
				__m128i  __128i;
				uint64_t __ui64[2];
			};

			XNG_INLINE __m128i morton_compact3_low(__m128i code)
			{
				static const __m128i mask0 = _mm_set_epi32(0x0, 0x0, 0x12492492, 0x49249249);
				static const __m128i mask1 = _mm_set_epi32(0x0, 0x0, 0x30C30C30, 0xC30C30C3);
				static const __m128i mask2 = _mm_set_epi32(0x0, 0x0, 0x100F00F0, 0x0F00F00F);
				static const __m128i mask3 = _mm_set_epi32(0x0, 0x0, 0x001F0000, 0xFF0000FF);
				static const __m128i mask4 = _mm_set_epi32(0x0, 0x0, 0x001F0000, 0x0000FFFF);

				__m128i t = _mm_and_si128(code, mask0);

				t = _mm_and_si128(_mm_xor_si128(t, _mm_srli_epi64(t, 2)), mask1);
				t = _mm_and_si128(_mm_xor_si128(t, _mm_srli_epi64(t, 4)), mask2);
				t = _mm_and_si128(_mm_xor_si128(t, _mm_srli_epi64(t, 8)), mask3);
				t = _mm_and_si128(_mm_xor_si128(t, _mm_srli_epi64(t, 16)), mask4);

				return t;
			}

			XNG_INLINE __m128i morton_split3_low(__m128i x)
			{

				__m128i mask21 = _mm_set_epi32(0x0, 0x0, 0x0, 0x1FFFFF);

				__m128i t = _mm_and_si128(x, mask21);

				static const __m128i mask0 = _mm_set_epi32(0x0, 0x0, 0x001F0000, 0x0000FFFF);
				static const __m128i mask1 = _mm_set_epi32(0x0, 0x0, 0x001F0000, 0xFF0000FF);
				static const __m128i mask2 = _mm_set_epi32(0x0, 0x0, 0x100F00F0, 0x0F00f00F);
				static const __m128i mask3 = _mm_set_epi32(0x0, 0x0, 0x10C30C30, 0xC30C30C3);
				static const __m128i mask4 = _mm_set_epi32(0x0, 0x0, 0x12492492, 0x49249249);

				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 32)), mask0);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 16)), mask1);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 8)), mask2);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 4)), mask3);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 2)), mask4);

				return t;

			}

			XNG_INLINE __m128i morton_split3_low_high(__m128i x)
			{

				__m128i mask21 = _mm_set_epi32(0x0, 0x1FFFFF, 0x0, 0x1FFFFF);

				__m128i t = _mm_and_si128(x, mask21);

				static const __m128i mask0 = _mm_set_epi32(0x001F0000, 0x0000FFFF, 0x001F0000, 0x0000FFFF);
				static const __m128i mask1 = _mm_set_epi32(0x001F0000, 0xFF0000FF, 0x001F0000, 0xFF0000FF);
				static const __m128i mask2 = _mm_set_epi32(0x100F00F0, 0x0F00f00F, 0x100F00F0, 0x0F00f00F);
				static const __m128i mask3 = _mm_set_epi32(0x10C30C30, 0xC30C30C3, 0x10C30C30, 0xC30C30C3);
				static const __m128i mask4 = _mm_set_epi32(0x12492492, 0x49249249, 0x12492492, 0x49249249);

				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 32)), mask0);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 16)), mask1);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 8)), mask2);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 4)), mask3);
				t = _mm_and_si128(_mm_or_si128(t, _mm_slli_epi64(t, 2)), mask4);

				return t;

			}

		}

		// Morton code obtained interleaving as in zyx

		XNG_INLINE morton_code morton_encode3(const morton_unpacked & m)
		{
			__m128i xiyi = detail::morton_split3_low_high(_mm_set_epi32(0x0, m.y, 0x0, m.x));
			__m128i zi = detail::morton_split3_low(_mm_set_epi32(0x0, 0x0, 0x0, m.z));

			__m128i yi = _mm_shuffle_epi32(xiyi, _MM_SHUFFLE(3, 2, 3, 2));

			yi = _mm_slli_epi64(yi, 1);
			zi = _mm_slli_epi64(zi, 2);

			detail::sse_register r = { _mm_or_si128(_mm_or_si128(xiyi, yi), zi) };

			return r.__ui64[0];
		}

		XNG_INLINE morton_unpacked morton_decode3(morton_code code)
		{
			detail::sse_register rx, ry, rz;

			rx.__ui64[0] = code;

			ry = { _mm_srli_epi64(rx.__128i, 1) };
			rz = { _mm_srli_epi64(rx.__128i, 2) };

			rx = { detail::morton_compact3_low(rx.__128i) };
			ry = { detail::morton_compact3_low(ry.__128i) };
			rz = { detail::morton_compact3_low(rz.__128i) };

			return morton_unpacked{ static_cast<uint32_t>(rx.__ui64[0]), static_cast<uint32_t>(ry.__ui64[0]), static_cast<uint32_t>(rz.__ui64[0]) };
		}

#endif

		namespace detail
		{
			XNG_INLINE morton_code morton_compact3(morton_code code)
			{
				uint64_t t = code & 0x1249249249249249ULL;

				t = (t ^ (t >> 2)) & 0x30C30C30C30C30C3ULL;
				t = (t ^ (t >> 4)) & 0x100F00F00F00F00FULL;
				t = (t ^ (t >> 8)) & 0x1F0000FF0000FFULL;
				t = (t ^ (t >> 16)) & 0x1F00000000FFFFULL;

				return t;
			}

			XNG_INLINE morton_code morton_split3(uint32_t x)
			{
				uint64_t t = x & 0x1FFFFF;

				t = (t | t << 32) & 0x1F00000000FFFF;
				t = (t | t << 16) & 0x1F0000FF0000FF;
				t = (t | t << 8) & 0x100F00F00F00F00F;
				t = (t | t << 4) & 0x10C30C30C30C30C3;
				t = (t | t << 2) & 0x1249249249249249;

				return t;
			}
		}

		XNG_INLINE morton_code morton_encode3(const morton_unpacked & m)
		{
			return detail::morton_split3(m.x) |
				detail::morton_split3(m.y) << 1 |
				detail::morton_split3(m.z) << 2;
		}

		XNG_INLINE morton_unpacked morton_decode3(morton_code code)
		{
			return morton_unpacked{
				detail::morton_compact3(code),
				detail::morton_compact3(code >> 1),
				detail::morton_compact3(code >> 2)
			};
		}
	}
}