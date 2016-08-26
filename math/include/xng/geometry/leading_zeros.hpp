#pragma once

#include <cstdint>

#define XNG_BSR_INVALID (0xFFFFFFFF)

namespace xng
{
	namespace math
	{
#ifdef XNG_WIN32

		XNG_INLINE uint32_t leading_zeros(uint64_t x)
		{
			union
			{
				uint64_t ui64;
				uint32_t ui32[2];
			} t = { x };

			unsigned long bsrHigh;
			unsigned long bsrLow;

			unsigned char okHigh = _BitScanReverse(&bsrHigh, t.ui32[1]);
			unsigned char okLow = _BitScanReverse(&bsrLow, t.ui32[0]);

			unsigned int lz;

			if (okHigh)
			{
				lz = 31 - bsrHigh;
			}
			else if (okLow)
			{
				lz = 63 - bsrLow;
			}
			else
			{
				lz = 64;
			}

			return lz;
		}

		XNG_INLINE uint32_t most_significant_one(uint64_t x)
		{
			union
			{
				uint64_t ui64;
				uint32_t ui32[2];
			} t = { x };

			unsigned long bsrHigh;
			unsigned long bsrLow;

			unsigned char okHigh = _BitScanReverse(&bsrHigh, t.ui32[1]);
			unsigned char okLow = _BitScanReverse(&bsrLow, t.ui32[0]);

			unsigned int bsr;

			if (okHigh)
			{
				bsr = bsrHigh + 32;
			}
			else if (okLow)
			{
				bsr = bsrLow;
			}
			else
			{
				bsr = XNG_BSR_INVALID;
			}

			return bsr;
		}

#endif
	}
}