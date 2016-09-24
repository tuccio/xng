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

			font(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
			~font(void);

			image_ptr get_image(void) const;
			void set_image(image_ptr image);

			const glyph_map & get_glyph_map(void) const;
			void set_glyph_map(const glyph_map & glyphs);

			uint32_t get_point_size(void) const;
			void set_point_size(uint32_t pointSize);

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