#include <xng/graphics/font.hpp>

namespace xng
{
	namespace graphics
	{
		struct text_vertex
		{
			math::float2 position;
			math::float2 texcoord;
		};

		struct text_info
		{
			uint32_t numGlyphs;
			uint32_t numTriangles;
			uint32_t numVertices;
			uint32_t width;
			uint32_t height;
		};

		template <typename String, typename OutputIterator>
		bool make_text(
			const font_ptr fnt,
			String str, 
			OutputIterator out,
			const math::int2 & padding = math::int2(0),
			const math::int2 & spacing = math::int2(0),
			text_info * pInfo = nullptr)
		{
			if (fnt && fnt->load())
			{
				using namespace xng::math;

				text_info info = {};

				std::wstring wstr;
				core::string_convert(str, wstr);

				int2 offset(0);

				const glyph_map & glyphs = fnt->get_glyph_map();

				for (wchar_t c : wstr)
				{
					const glyph * g = glyphs[c];

					if (g)
					{
						int2 glyphSize = g->size;

						text_vertex v[4];

						v[0].position   = (float2)(offset + g->offset);
						v[0].texcoord   = g->uv0;

						v[1].position.x = v[0].position.x;
						v[1].position.y = v[0].position.y + glyphSize.y;

						v[1].texcoord   = float2(g->uv0.x, g->uv1.y);

						v[2].position.x = v[0].position.x + glyphSize.x;
						v[2].position.y = v[1].position.y;

						v[2].texcoord   = g->uv1;

						v[3].position.x = v[2].position.x;
						v[3].position.y = v[0].position.y;

						v[3].texcoord   = float2(g->uv1.x, g->uv0.y);

						if (padding != int2(0))
						{
							float2 pad = (float2)padding;

							v[0].position   = v[0].position - pad;
							v[2].position   = v[2].position + pad;
							v[1].position.x = v[0].position.x;
							v[1].position.y = v[2].position.y;
							v[3].position.x = v[2].position.x;
							v[3].position.y = v[0].position.y;

							float2 invTexelSize = (g->uv1 - g->uv0) / (float2)glyphSize;
							float2 texPadding = pad * invTexelSize;

							v[0].texcoord   = v[0].texcoord - texPadding;
							v[2].texcoord   = v[2].texcoord + texPadding;
							v[1].texcoord.x = v[0].texcoord.x;
							v[1].texcoord.y = v[2].texcoord.y;
							v[3].texcoord.x = v[2].texcoord.x;
							v[3].texcoord.y = v[0].texcoord.y;
						}

						*out++ = v[0];
						*out++ = v[1];
						*out++ = v[2];

						*out++ = v[2];
						*out++ = v[3];
						*out++ = v[0];

						offset.x += g->advance + spacing.x;

						++info.numGlyphs;
						info.height = max<uint32_t>(info.height, glyphSize.y);
						info.width += glyphSize.x;
					}
				}


				if (pInfo)
				{
					info.numTriangles = info.numGlyphs * 2;
					info.numVertices  = info.numGlyphs * 6;
					*pInfo = info;
				}

				return true;
			}
			else
			{
				return false;
			}
		}
	}
}