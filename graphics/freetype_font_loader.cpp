#ifdef XNG_FREETYPE

#include <xng/graphics/freetype_font_loader.hpp>
#include <xng/graphics/font.hpp>
#include <xng/math.hpp>

#include <omp.h>

using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

#define pixel(Data, I, J, Cols) (Data)[(I) * (Cols) + (J)]

struct load_data
{
	glyph_map     * glyphs;
	const wchar_t * charset;
	FT_Face         face;
	uint32_t        size;
	uint32_t        supersampleFactor;
};

int8_t sdf_search_radius(const uint8_t * src, uint32_t w, uint32_t h, int32_t i, int32_t j, int8_t radius)
{
	bool allWhite = true;
	bool allBlack = true;
	bool isWhite  = pixel(src, i, j, w) != 0;

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

void sdf_calculate_radar(uint8_t * dst, const uint8_t * src, uint32_t w, uint32_t h, int8_t spreadFactor)
{
	const float invNorm = 255.f / (spreadFactor + spreadFactor);

	for (int32_t i = 0; i < h; ++i)
	{
		for (int32_t j = 0; j < w; ++j)
		{
			int32_t maxRadius = min<int32_t>(spreadFactor, max<int32_t>(max(max<int32_t>(i, h - i), j), w - j));

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

bool load_image(image * img, const load_data * data)
{
	const int8_t   SpreadFactor         = 4;
	const uint32_t SupersampledFontSize = data->size * data->supersampleFactor;

	const int8_t   Padding = SpreadFactor * data->supersampleFactor;

	uint32_t totalWidth  = 0;
	uint32_t totalHeight = 0;
	uint32_t numGlyphs   = 0;

	FT_Set_Pixel_Sizes(data->face, 0, SupersampledFontSize);

	for (const wchar_t *c = data->charset; *c != 0; ++c)
	{
		if (FT_Load_Char(data->face, *c, FT_LOAD_RENDER))
		{
			continue;
		}

		++numGlyphs;

		totalWidth += data->face->glyph->bitmap.width;
		totalHeight = max<uint32_t>(totalHeight, data->face->glyph->bitmap.rows);
	}

	uint32_t supersampledWidth  = totalWidth + 2 * Padding * (numGlyphs + 1);
	uint32_t supersampledHeight = totalHeight + 2 * Padding;

	uint32_t imageWidth  = supersampledWidth / data->supersampleFactor;
	uint32_t imageHeight = supersampledHeight / data->supersampleFactor;

	std::vector<uint8_t> upsampledImg(supersampledWidth * supersampledHeight);

	int2 currentOffset(SpreadFactor);

	float2 invTexelSize = { 1.f / supersampledWidth, 1.f / supersampledHeight };
	//float2 invTexelSize = { 1.f / imageWidth, 1.f / imageHeight };

	for (const wchar_t * c = data->charset; *c != 0; ++c)
	{
		if (FT_Load_Char(data->face, *c, FT_LOAD_RENDER))
		{
			continue;
		}

		int2 glyphSize(data->face->glyph->bitmap.width, data->face->glyph->bitmap.rows);

		/*glyph * g = (*data->glyphs)[*c];

		g->code   = *c;
		g->width  = data->face->glyph->advance.x / 64;
		g->offset = int2(data->face->glyph->bitmap_left, data->face->glyph->bitmap_top);
		g->x0     = currentOffset;
		g->x1     = currentOffset + glyphSize;
		g->uv0    = (float2)g->x0 * invTexelSize;
		g->uv1    = (float2)g->x1 * invTexelSize;*/

		int2 textureOffset = {
			currentOffset.x,
			((int32_t)supersampledHeight - glyphSize.y) / 2
		};

		glyph * g = (*data->glyphs)[*c];

		g->code   = *c;
		g->width  = (data->face->glyph->advance.x / 64) / data->supersampleFactor;
		g->offset = int2(data->face->glyph->bitmap_left, data->face->glyph->bitmap_top) / data->supersampleFactor;
		g->x0     = currentOffset / data->supersampleFactor;
		g->x1     = currentOffset / data->supersampleFactor + glyphSize / data->supersampleFactor;
		g->uv0    = (float2)textureOffset * invTexelSize;
		g->uv1    = (float2)(textureOffset + glyphSize) * invTexelSize;

		for (int i = 0; i < glyphSize.y; ++i)
		{
			for (int j = 0; j < glyphSize.x; ++j)
			{
				pixel(upsampledImg, textureOffset.y + i, textureOffset.x + j, supersampledWidth) = pixel(data->face->glyph->bitmap.buffer, i, j, glyphSize.x);
			}
		}

		currentOffset.x += glyphSize.x + 2 * Padding;
	}

	std::vector<uint8_t> upsampledSDF(supersampledWidth * supersampledHeight);
	sdf_calculate_radar(upsampledSDF.data(), upsampledImg.data(), supersampledWidth, supersampledHeight, Padding);

	/*img->create(XNG_IMAGE_FORMAT_A8_UINT, supersampledWidth, supersampledHeight, upsampledImg.data());
	img->write_png_file("upsampled.png");*/

	upsampledImg.clear();
	upsampledImg.shrink_to_fit();

	/*img->create(XNG_IMAGE_FORMAT_A8_UINT, supersampledWidth, supersampledHeight, upsampledSDF.data());
	img->write_png_file("upsampledsdf.png");*/

	img->create(XNG_IMAGE_FORMAT_A8_UINT, imageWidth, imageHeight);

	// Copy and downsample

	uint8_t * finalImage = img->data();

	int2 scaleFactor = {
		(int) std::ceil(supersampledWidth / imageWidth),
		(int) std::ceil(supersampledHeight / imageHeight)
	};

	for (int32_t i = 0; i < img->get_height(); ++i)
	{
		for (int32_t j = 0; j < img->get_width(); ++j)
		{
			uint32_t avg = 0;
			uint32_t numSamples = 0;

			int iu = i * data->supersampleFactor;
			int ju = j * data->supersampleFactor;			

			for (int n = 0; n < scaleFactor.x; ++n)
			{
				for (int m = 0; m < scaleFactor.y; ++m)
				{
					int r = iu + n;
					int c = ju + m;

					if (r >= 0 && r < supersampledHeight &&
						c >= 0 && c < supersampledWidth)
					{
						avg += pixel(upsampledSDF, r, c, supersampledWidth);
						++numSamples;
					}
				}
			}

			avg /= numSamples;

			pixel(finalImage, i, j, img->get_width()) = avg;
		}
	}

	//img->write_png_file("sdf.png");

	return true;
}

bool load_font(resource_loader * loader, FT_Library library, font * fnt)
{
	const char * name = fnt->get_name();
	auto optSize = fnt->get_parameters().get_optional<uint32_t>("size");

	const uint32_t UpsampleFactor = 4;

	uint32_t size       = optSize ? optSize.get() : 16;
	uint32_t renderSize = size * UpsampleFactor;

	FT_Face face;

	if (FT_Error error = FT_New_Face(library, name, 0, &face))
	{
		XNG_LOG("freetype_font_loader", XNG_LOG_STREAM() << "FT_New_Face failed on file \"" << name << "\" with error " << std::hex << error);
		return false;
	}

	image_ptr img = resource_factory::get_singleton()->create<image>("", make_resource_parameters("generate_mipmaps", true), resource_loader_ptr(loader));

	glyph_map glyphs;

	load_data data;

	data.glyphs            = &glyphs;
	data.charset           = make_default_charset();
	data.face              = face;
	data.supersampleFactor = UpsampleFactor;
	data.size              = size;

	bool loaded = img->load(&data);

	fnt->set_image(img);
	fnt->set_point_size(size);
	fnt->set_glyph_map(glyphs);

	FT_Done_Face(face);

	return loaded;
}

bool freetype_font_loader::load(resource * r, const void * userdata)
{
	if (!r)
	{
		return false;
	}

	font * f = dynamic_cast<font*>(r);

	if (!f)
	{
		image * i = dynamic_cast<image*>(r);

		if (!i)
		{
			XNG_LOG("freetype_font_loader", "Unrecognized resource type.");
			return false;
		}
		else
		{
			return load_image(i, static_cast<const load_data*>(userdata));
		}
	}
	else
	{
		freetype_initializer ftlib;

		if (ftlib && load_font(this, ftlib, f))
		{
			return true;
		}

		return false;
	}
	
}

void freetype_font_loader::unload(resource * r)
{
	font * f = dynamic_cast<font*>(r);

	if (!f)
	{
		image * i = dynamic_cast<image*>(r);

		if (!i)
		{
			i->clear();
		}
	}
	else
	{
		f->clear();
	}
}

#endif