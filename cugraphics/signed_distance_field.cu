#include "export.hpp"

#include <cuda.h>

#include <cstdint>
#include <limits>

#define pixel(Data, I, J, Cols) (Data)[(I) * (Cols) + (J)]

extern "C" void XNG_EXPORT sdf_calculate(uint8_t * dst, const uint8_t * src, uint32_t w, uint32_t h, int8_t spreadFactor)
{
	// TODO	
}