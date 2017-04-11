#include <xng/graphics/image.hpp>
#include <xng/core.hpp>

#include <utility>
#include <FreeImage.h>

using namespace xng::graphics;
using namespace xng::res;
using namespace xng::os;

const char * image::resource_type = "image";

image::image(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
    resource(name, params, loader, owner) {}

image::~image(void)
{
    clear();
}

bool image::load_impl(const void * userdata)
{
    return read_file(get_name());
}

void image::unload_impl(void)
{
    clear();
}

size_t image::calculate_size_impl(void)
{
    return m_data.size();
}

void image::clear(void)
{
    m_data.clear();
    m_data.shrink_to_fit();
}

bool image::create(xng_image_format format, uint32_t width, uint32_t height, const void * data)
{
    size_t size = width * height * get_format_stride(format);

    m_data.resize(size);

    m_width  = width;
    m_height = height;

    m_format = format;

    // Align scan width to 4 bytes
    m_scanWidth = (width * get_format_stride(m_format) + 3) / 4;

    if (data)
    {
        memcpy(&m_data[0], data, size);
    }

    return true;
}

bool freeimage_write_file(const path & filename, const std::vector<uint8_t> & data, uint32_t width, uint32_t height, uint32_t scanWidth, xng_image_format format, FREE_IMAGE_FORMAT fif)
{
    bool success = false;

    FIBITMAP * bitmap = FreeImage_ConvertFromRawBits(
        (BYTE*)&data[0], width, height,
        scanWidth, 8 * get_format_stride(format),
        FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK,
        TRUE);

    if (bitmap)
    {
#ifdef XNG_WIN32
        success = TRUE == FreeImage_SaveU(fif, bitmap, filename.c_str());
#else
        success = TRUE == FreeImage_Save(fif, bitmap, filename.c_str());
#endif

        FreeImage_Unload(bitmap);
    }

    return success;
}

bool freeimage_read_file(const path & filename, std::vector<uint8_t> & data, uint32_t & width, uint32_t & height, uint32_t & scanWidth, xng_image_format & format, FREE_IMAGE_FORMAT fif)
{
    bool success = false;

#ifdef XNG_WIN32
    FIBITMAP * bitmap = FreeImage_LoadU(fif, filename.c_str());
#else
    FIBITMAP * bitmap = FreeImage_Load(fif, filename.c_str());
#endif

    if (bitmap)
    {
        width     = FreeImage_GetWidth(bitmap);
        height    = FreeImage_GetHeight(bitmap);
        scanWidth = FreeImage_GetPitch(bitmap);

        int bpp = FreeImage_GetBPP(bitmap);

        format = XNG_IMAGE_FORMAT_UNKNOWN;

        FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(bitmap);

        switch (imageType)
        {
        case FIT_BITMAP:
            switch (bpp)
            {
            case 8:
                format = XNG_IMAGE_FORMAT_A8_UINT;
                break;
            case 24:
                format = XNG_IMAGE_FORMAT_R8G8B8_UINT;
                break;
            case 32:
                format = XNG_IMAGE_FORMAT_R8G8B8A8_UINT;
                break;
            }
            break;
        }

        data.resize(height * scanWidth);

        bool bgr = false;

        bgr = FIF_TARGA;

        FreeImage_ConvertToRawBits(
            &data[0],
            bitmap, scanWidth, bpp,
            FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK,
            TRUE);

        FreeImage_Unload(bitmap);

        if (bpp == 24 && bgr)
        {
            struct _24bppRGB { uint8_t r, g, b; };
            struct _24bppBGR { uint8_t b, g, r; };

            std::transform((const _24bppBGR *)&data.front(), (const _24bppBGR *)(&data.front() + data.size()), (_24bppRGB *)&data.front(),
                [](const _24bppBGR & pixel)
            {
                _24bppRGB out;

                out.r = pixel.r;
                out.g = pixel.g;
                out.b = pixel.b;

                return out;
            });
        }

        success = true;
    }

    return success;
}

bool image::read_file(const path & filename)
{
#ifdef XNG_WIN32
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(filename.c_str());
#else
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
#endif

    return fif == FIF_UNKNOWN ? false :
        freeimage_read_file(
            filename,
            m_data,
            m_width,
            m_height,
            m_scanWidth,
            m_format,
            fif
        );
}

bool image::write_file(const path & filename) const
{
#ifdef XNG_WIN32
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(filename.c_str());
#else
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
#endif

    return fif == FIF_UNKNOWN ? false :
        freeimage_write_file(
            filename,
            m_data,
            m_width,
            m_height,
            m_scanWidth,
            m_format,
            fif
        );
}

bool image::write_png_file(const path & filename) const
{
    return freeimage_write_file(
        filename,
        m_data,
        m_width,
        m_height,
        m_scanWidth,
        m_format,
        FIF_PNG
    );
}

bool image::read_png_file(const path & filename)
{
    return freeimage_read_file(
        filename,
        m_data,
        m_width,
        m_height,
        m_scanWidth,
        m_format,
        FIF_PNG
    );
}


//png_byte get_format_libpng_bitdepth(xng_image_format format);
//png_byte get_format_libpng_colortype(xng_image_format format);
//
//xng_image_format get_libpng_format(png_byte bit_depth, png_byte color_type);
//
//bool image::write_png_file(const path & filename) const
//{
//    bool success = false;
//
//    uint32_t bitDepth = get_format_libpng_bitdepth(m_format);
//
//    if (bitDepth == 0)
//    {
//        XNG_LOG("image::write_png_file", "Current image format cannot be exported to PNG.");
//        return false;
//    }
//    
//    FILE * fp = fopen(filename.string().c_str(), "wb");
//
//    if (fp)
//    {
//        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//
//        if (png_ptr)
//        {
//            png_infop info_ptr = png_create_info_struct(png_ptr);
//
//            if (info_ptr)
//            {
//                std::vector<png_bytep> rows(m_height);
//
//                uint32_t stride = get_format_stride(m_format) * get_width();
//
//                for (int i = 0; i < m_height; ++i)
//                {
//                    rows[i] = (png_bytep) data() + i * stride;
//                }
//
//                if (!setjmp(png_jmpbuf(png_ptr)))
//                {
//                    png_init_io(png_ptr, fp);
//
//                    png_set_IHDR(png_ptr, info_ptr, m_width, m_height,
//                        bitDepth,
//                        get_format_libpng_colortype(m_format),
//                        PNG_INTERLACE_NONE,
//                        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//
//                    png_write_info(png_ptr, info_ptr);
//                    png_write_image(png_ptr, &rows[0]);
//                    png_write_end(png_ptr, info_ptr);
//
//                    bool success = true;
//                }
//                else
//                {
//                    XNG_LOG("image::write_png_file", "An error occurred while writing the PNG file.");
//                }
//
//            }
//
//            png_destroy_write_struct(&png_ptr, &info_ptr);
//        }
//
//        fclose(fp);
//    }
//    else
//    {
//        XNG_LOG("image::write_png_file", "Cannot open the file in write mode.");
//    }
//
//    return false;    
//}
//
//bool image::read_png_file(const path & filename)
//{
//    bool success = false;
//
//    FILE * fp = fopen(filename.string().c_str(), "rb");
//
//    if (fp)
//    {
//        png_byte header[8];
//        fread(header, 1, 8, fp);
//
//        if (png_sig_cmp(header, 0, 8))
//        {
//            XNG_LOG("image::read_png_file", "File is not recognized as a PNG file.");
//        }
//        else
//        {
//            png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//
//            if (png_ptr)
//            {
//                png_infop info_ptr = png_create_info_struct(png_ptr);
//
//                if (info_ptr)
//                {
//                    png_init_io(png_ptr, fp);
//                    png_set_sig_bytes(png_ptr, 8);
//
//                    png_read_info(png_ptr, info_ptr);
//
//                    m_width  = png_get_image_width(png_ptr, info_ptr);
//                    m_height = png_get_image_height(png_ptr, info_ptr);
//
//                    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
//                    png_byte bit_depth  = png_get_bit_depth(png_ptr, info_ptr);
//
//                    m_format = get_libpng_format(bit_depth, color_type);
//
//                    if (m_format != XNG_IMAGE_FORMAT_UNKNOWN)
//                    {
//                        int number_of_passes = png_set_interlace_handling(png_ptr);
//                        png_read_update_info(png_ptr, info_ptr);
//
//                        std::vector<png_bytep> rows(m_height);
//
//                        uint32_t stride = get_format_stride(m_format) * get_width();
//
//                        m_data.resize(stride * m_height);
//
//                        for (int i = 0; i < m_height; ++i)
//                        {
//                            rows[i] = (png_bytep)data() + i * stride;
//                        }
//
//                        if (!setjmp(png_jmpbuf(png_ptr)))
//                        {
//                            png_read_image(png_ptr, &rows[0]);
//                            success = true;
//                        }
//                        else
//                        {
//                            XNG_LOG("image::read_png_file", "An error occurred while reading the PNG file.");
//                        }
//                    }
//                }
//
//                png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
//            }
//        }
//
//        fclose(fp);
//    }
//    else
//    {
//        XNG_LOG("image::read_png_file", "Cannot open the file in read mode.");
//    }
//
//    return success;
//}

uint32_t image::get_scan_width(void) const
{
    return m_scanWidth;
}

uint32_t xng::graphics::get_format_stride(xng_image_format format)
{
    uint32_t stride;

    switch (format)
    {

    case XNG_IMAGE_FORMAT_R16G16B16_FLOAT:
        stride = 6;
        break;

    case XNG_IMAGE_FORMAT_R32G32B32_FLOAT:
        stride = 12;
        break;

    case XNG_IMAGE_FORMAT_R8G8B8_UINT:
        stride = 3;
        break;

    case XNG_IMAGE_FORMAT_R16G16B16A16_FLOAT:
        stride = 8;
        break;

    case XNG_IMAGE_FORMAT_R32G32B32A32_FLOAT:
        stride = 16;
        break;

    case XNG_IMAGE_FORMAT_R8G8B8A8_UINT:
    case XNG_IMAGE_FORMAT_R32_FLOAT:
        stride = 4;
        break;

    case XNG_IMAGE_FORMAT_A8_UINT:
    case XNG_IMAGE_FORMAT_R8_INT:
        stride = 1;
        break;

    default:
        stride = 0;
        break;

    }

    return stride;
}

//png_byte get_format_libpng_bitdepth(xng_image_format format)
//{
//    uint32_t depth;
//
//    switch (format)
//    {
//    case XNG_IMAGE_FORMAT_R8G8B8_UINT:
//    case XNG_IMAGE_FORMAT_R8G8B8A8_UINT:
//    case XNG_IMAGE_FORMAT_A8_UINT:
//    case XNG_IMAGE_FORMAT_R8_INT:
//        depth = 8;
//        break;
//
//    default:
//        depth = 0;
//        break;
//
//    }
//
//    return depth;
//}
//
//png_byte get_format_libpng_colortype(xng_image_format format)
//{
//    uint32_t type;
//
//    switch (format)
//    {
//    case XNG_IMAGE_FORMAT_R8G8B8_UINT:
//        type = 2;
//        break;
//
//    case XNG_IMAGE_FORMAT_R8G8B8A8_UINT:
//        type = 6;
//        break;
//
//    default:
//    case XNG_IMAGE_FORMAT_A8_UINT:
//    case XNG_IMAGE_FORMAT_R8_INT:
//        type = 0;
//        break;
//
//    }
//
//    return type;
//}
//
//xng_image_format get_libpng_format(png_byte bit_depth, png_byte color_type)
//{
//    xng_image_format format = XNG_IMAGE_FORMAT_UNKNOWN;
//
//    if (bit_depth == 8)
//    {
//        switch (color_type)
//        {
//        case 2:
//            format = XNG_IMAGE_FORMAT_R8G8B8_UINT;
//            break;
//
//        case 6:
//            format = XNG_IMAGE_FORMAT_R8G8B8A8_UINT;
//            break;
//
//        default:
//        case 0:
//            format = XNG_IMAGE_FORMAT_A8_UINT;
//            break;
//
//        }
//    }
//
//    return format;
//}