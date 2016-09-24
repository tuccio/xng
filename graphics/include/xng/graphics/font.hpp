#pragma once

#include <xng/res.hpp>
#include <xng/graphics/glyph_map.hpp>
#include <xng/graphics/image.hpp>
#include <xng/core/string_convert.hpp>

#include <xng/os/path.hpp>

namespace xng
{
	namespace graphics
	{
		class font :
			public res::resource
		{

		public:

			static const char * resource_type;

			struct vertex
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

			font(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
			~font(void);

			image_ptr get_image(void) const;
			void set_image(image_ptr image);

			const glyph_map & get_glyph_map(void) const;
			void set_glyph_map(const glyph_map & glyphs);

			uint32_t get_point_size(void) const;
			void set_point_size(uint32_t pointSize);

			template <typename String, typename OutputIterator>
			void make_text(String str, OutputIterator out, text_info * pInfo, math::int2 padding = math::int2(0)) const
			{
				using namespace xng::math;

				text_info info = {};

				std::wstring wstr;
				core::string_convert(str, wstr);

				uint32_t offset = 0;

				for (wchar_t c : wstr)
				{
					const glyph * g = m_glyphs[c];

					if (g)
					{
						int2 glyphSize = g->x1 - g->x0;

						vertex v[4];

						v[0].position.x = offset - g->offset.x;
						v[0].position.y = -g->offset.y;

						v[0].texcoord   = g->uv0;

						v[1].position.x = v[0].position.x;
						v[1].position.y = v[0].position.y + glyphSize.y;

						v[1].texcoord   = float2(g->uv0.x, g->uv1.y);

						v[2].position.x = offset + glyphSize.x;
						v[2].position.y = v[1].position.y;

						v[2].texcoord   = g->uv1;

						v[3].position.x = v[2].position.x;
						v[3].position.y = v[0].position.y;

						v[3].texcoord = float2(g->uv1.x, g->uv0.y);

						if (padding != int2(0))
						{
							float2 pad = (float2) padding;

							v[0].position   = v[0].position - pad;
							v[2].position   = v[2].position + pad;
							v[1].position.x = v[0].position.x;
							v[1].position.y = v[2].position.y;
							v[3].position.x = v[2].position.x;
							v[3].position.y = v[0].position.y;

							float2 invTexelSize  = (g->uv1 - g->uv0) / (float2) glyphSize;
							float2 texPadding = pad * invTexelSize;

							v[0].texcoord   = v[0].texcoord - texPadding;
							v[2].texcoord   = v[2].texcoord + texPadding;
							v[1].texcoord.x = v[0].texcoord.x;
							v[1].texcoord.y = v[2].texcoord.y;
							v[3].texcoord.x = v[2].texcoord.x;
							v[3].texcoord.y = v[0].texcoord.y;
						}

						*out = v[0];
						*(++out) = v[1];
						*(++out) = v[2];

						*(++out) = v[2];
						*(++out) = v[3];
						*(++out) = v[0];

						++out;
						
						offset += g->width + padding.x;

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
			}

			bool write_file(const os::path & filename) const;
			bool read_file(const os::path & filename);

			void clear(void);

		protected:

			bool   load_impl(const void * userdata = nullptr) override;
			void   unload_impl(void) override;
			size_t calculate_size_impl(void) override;

		private:

			image_ptr m_image;
			glyph_map m_glyphs;
			uint32_t  m_pointSize;

		};

		typedef res::resource_ptr<font>           font_ptr;
		typedef res::basic_resource_manager<font> font_manager;

		const wchar_t * make_default_charset(void);
	}
}