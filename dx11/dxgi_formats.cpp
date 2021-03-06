#include <xng/dx11/dxgi_formats.hpp>

#include <cassert>

uint32_t xng::dx11::get_dxgi_format_byte_size(DXGI_FORMAT format)
{
    switch (format)
    {

    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R8_SINT:
        return 1;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R32_UINT:
        return 4;

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return 8;

    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return 16;

    default:
        assert(false && "get_dxgi_format_size unknown type");
        return 0;
    }
}

DXGI_FORMAT xng::dx11::get_dxgi_format(xng_image_format format)
{
    switch (format)
    {

    case XNG_IMAGE_FORMAT_R16G16B16_FLOAT:
    case XNG_IMAGE_FORMAT_R8G8B8_UINT:
    default:
        return DXGI_FORMAT_UNKNOWN;

    case XNG_IMAGE_FORMAT_R32G32B32_FLOAT:
        return DXGI_FORMAT_R32G32B32_FLOAT;

    case XNG_IMAGE_FORMAT_R16G16B16A16_FLOAT:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

    case XNG_IMAGE_FORMAT_R32G32B32A32_FLOAT:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    case XNG_IMAGE_FORMAT_R8G8B8A8_UINT:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    case XNG_IMAGE_FORMAT_A8_UINT:
        return DXGI_FORMAT_A8_UNORM;

    case XNG_IMAGE_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;

    case XNG_IMAGE_FORMAT_R8_INT:
        return DXGI_FORMAT_R8_SINT;

    }
}
