#ifdef XNG_FREETYPE

#include <xng/graphics/freetype_font_loader.hpp>
#include <xng/graphics/font.hpp>
#include <xng/graphics/acceleration_library.hpp>
#include <xng/math.hpp>

#include <omp.h>

using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

#define pixel(Data, I, J, Cols) (Data)[(I) * (Cols) + (J)]

typedef void (*sdf_calculate_t) (uint8_t *, const uint8_t *, uint32_t, uint32_t, int8_t);

struct load_data
{
	glyph_map     * glyphs;
	const wchar_t * charset;
	FT_Face         face;
	uint32_t        size;
	uint32_t        supersampleFactor;
	uint32_t        spreadFactor;
};

bool load_image(image * img, const load_data * data)
{
	acceleration_library * accelLib = nullptr;

	if (!(accelLib = acceleration_library::get_singleton()))
	{
		accelLib = xng_new acceleration_library;
	}

	sdf_calculate_t sdf_calculate = (sdf_calculate_t) accelLib->get_symbol_address("sdf_calculate");

	if (!sdf_calculate)
	{
		XNG_LOG("xng::graphics::freetype_font_loader", "Couldn't load sdf_calculate from the acceleration library.")
		return false;
	}

	const int8_t SpreadFactor = data->spreadFactor;
	const int8_t Padding      = SpreadFactor * data->supersampleFactor;

	uint32_t totalWidth  = 0;
	uint32_t totalHeight = 0;
	uint32_t numGlyphs   = 0;

	FT_Matrix scale = {
		(FT_Fixed) (0x10000L * data->supersampleFactor), 0,
		0, (FT_Fixed) (0x10000L * data->supersampleFactor)
	};

	FT_Set_Pixel_Sizes(data->face, 0, data->size);

	for (const wchar_t *c = data->charset; *c != 0; ++c)
	{
		if (FT_Load_Char(data->face, *c, 0))
		{
			continue;
		}

		++numGlyphs;

		totalWidth += data->supersampleFactor * data->face->glyph->metrics.width / 64;
		totalHeight = max<uint32_t>(totalHeight, data->supersampleFactor *  data->face->glyph->metrics.height / 64);
	}

	uint32_t supersampledWidth  = totalWidth + 2 * Padding * (numGlyphs + 1);
	uint32_t supersampledHeight = totalHeight + 2 * Padding;

	uint32_t imageWidth  = supersampledWidth / data->supersampleFactor;
	uint32_t imageHeight = supersampledHeight / data->supersampleFactor;

	std::vector<uint8_t> upsampledImg(supersampledWidth * supersampledHeight);

	int2 currentOffset(Padding);

	float2 invTexelSize = { 1.f / supersampledWidth, 1.f / supersampledHeight };

	for (const wchar_t * c = data->charset; *c != 0; ++c)
	{
		FT_Set_Transform(data->face, nullptr, nullptr);
		if (FT_Load_Char(data->face, *c, 0))
		{
			continue;
		}

		glyph * g = (*data->glyphs)[*c];
		FT_GlyphSlot ftGlyph = data->face->glyph;

		g->code    = *c;
		g->advance = ftGlyph->advance.x / 64;
		g->size    = int2(data->face->glyph->metrics.width, data->face->glyph->metrics.height) / 64;
		g->offset  = int2(data->face->glyph->metrics.horiBearingX, -data->face->glyph->metrics.horiBearingY) / 64;

		FT_Set_Transform(data->face, &scale, nullptr);
		FT_Load_Char(data->face, *c, FT_LOAD_RENDER);

		int2 glyphSize(data->face->glyph->bitmap.width, data->face->glyph->bitmap.rows);

		int2 textureOffset = {
			currentOffset.x,
			((int32_t)supersampledHeight - glyphSize.y) / 2
		};

		for (int i = 0; i < glyphSize.y; ++i)
		{
			memcpy(
				&pixel(upsampledImg, textureOffset.y + i, textureOffset.x, supersampledWidth),
				&pixel(data->face->glyph->bitmap.buffer, i, 0, glyphSize.x),
				glyphSize.x);
		}

		g->uv0 = (float2)textureOffset * invTexelSize;
		g->uv1 = (float2)(textureOffset + glyphSize) * invTexelSize;

		currentOffset.x += glyphSize.x + 2 * Padding;
	}

	std::vector<uint8_t> upsampledSDF(supersampledWidth * supersampledHeight);
	sdf_calculate(upsampledSDF.data(), upsampledImg.data(), supersampledWidth, supersampledHeight, SpreadFactor);

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

	auto optSize     = fnt->get_parameters().get_optional<uint32_t>("size");
	auto optSpread   = fnt->get_parameters().get_optional<uint32_t>("spread_factor");
	auto optUpsample = fnt->get_parameters().get_optional<uint32_t>("samples");

	const uint32_t SpreadFactor   = optSpread ? optSpread.get() : 8;
	const uint32_t UpsampleFactor = optUpsample ? optUpsample.get() : 4;

	const uint32_t FontSize = optSize ? optSize.get() : 16;

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
	data.spreadFactor      = SpreadFactor;
	data.size              = FontSize;

	bool loaded = img->load(&data);

	fnt->set_image(img);
	fnt->set_point_size(FontSize);
	fnt->set_spread_factor(SpreadFactor);
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