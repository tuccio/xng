#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/graphics/image.hpp>
#include <cstdint>

namespace xng
{
    namespace dx11
    {
        uint32_t get_dxgi_format_byte_size(DXGI_FORMAT format);
        DXGI_FORMAT get_dxgi_format(xng_image_format format);
    }
}