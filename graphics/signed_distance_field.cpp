#include <xng/core/export.hpp>

#include <cstdint>
#include <limits>
#include <algorithm>

#define pixel(Data, I, J, Cols) (Data)[(I) * (Cols) + (J)]

int8_t sdf_search_radius(const uint8_t * src, uint32_t w, uint32_t h, int32_t i, int32_t j, int8_t radius)
{
	bool allWhite = true;
	bool allBlack = true;
	bool isWhite = pixel(src, i, j, w) != 0;

	for (int32_t m = i - radius; m < i + radius; ++m)
	{
		if (m >= 0 && m < h)
		{
			for (int32_t n = j - radius; n < j + radius; ++n)
			{
				if (n >= 0 && n < w)
				{
					uint8_t px = pixel(src, m, n, w);

					if (px == 0)
					{
						allWhite = false;
					}
					else
					{
						allBlack = false;
					}
				}
			}
		}
	}

	if (isWhite)
	{
		return allWhite ? CHAR_MAX : 1 - radius;
	}
	else
	{
		return allBlack ? CHAR_MAX : radius;
	}
}

extern "C" void XNG_EXPORT sdf_calculate(uint8_t * dst, const uint8_t * src, uint32_t w, uint32_t h, int8_t spreadFactor)
{
	const float invNorm = 255.f / (spreadFactor + spreadFactor);

	for (int32_t i = 0; i < h; ++i)
	{
		for (int32_t j = 0; j < w; ++j)
		{
			int32_t maxRadius = std::min<int32_t>(spreadFactor, std::max<int32_t>(std::max(std::max<int32_t>(i, h - i), j), w - j));

			uint8_t & px = pixel(dst, i, j, w);

			int8_t result = spreadFactor;

			for (int32_t radius = 1; radius < maxRadius; ++radius)
			{
				int8_t search = sdf_search_radius(src, w, h, i, j, radius);

				if (search != CHAR_MAX)
				{
					result = search;
					break;
				}
			}

			float mappedValue = ((float)-result + spreadFactor) * invNorm;
			px = mappedValue;
		}
	}
}