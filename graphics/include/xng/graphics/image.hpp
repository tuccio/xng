#pragma once

#include <xng/res.hpp>
#include <xng/os.hpp>

#include <cstdint>
#include <vector>

enum xng_image_format
{
	XNG_IMAGE_FORMAT_UNKNOWN,
	XNG_IMAGE_FORMAT_R8G8B8A8_UINT,
	XNG_IMAGE_FORMAT_R16G16B16A16_FLOAT,
	XNG_IMAGE_FORMAT_R32G32B32A32_FLOAT,
	XNG_IMAGE_FORMAT_R8G8B8_UINT,
	XNG_IMAGE_FORMAT_R16G16B16_FLOAT,
	XNG_IMAGE_FORMAT_R32G32B32_FLOAT,
	XNG_IMAGE_FORMAT_A8_UINT,
	XNG_IMAGE_FORMAT_R8_INT,
	XNG_IMAGE_FORMAT_R32_FLOAT
};

namespace xng
{
	namespace graphics
	{
		class image :
			public res::resource
		{

		public:

			static const char * resource_type;

			image(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
			~image(void);

			bool create(xng_image_format format, uint32_t width, uint32_t height, const void * data = nullptr);

			XNG_INLINE uint8_t * data(void) { return &m_data[0]; }
			XNG_INLINE const uint8_t * data(void) const { return &m_data[0]; }

			XNG_INLINE xng_image_format get_format(void) const { return m_format; }

			XNG_INLINE uint32_t get_width(void) const { return m_width; }
			XNG_INLINE uint32_t get_height(void) const { return m_height; }

			void clear(void);

			bool write_file(const os::path & filename) const;
			bool read_file(const os::path & filename);

			bool write_png_file(const os::path & filename) const;
			bool read_png_file(const os::path & filename);

			uint32_t get_scan_width(void) const;

		protected:

			bool   load_impl(const void * userdata = nullptr) override;
			void   unload_impl(void) override;
			size_t calculate_size_impl(void) override;

		private:

			std::vector<uint8_t> m_data;

			xng_image_format     m_format;

			uint32_t             m_width;
			uint32_t             m_height;
			uint32_t             m_scanWidth;

		};

		typedef res::resource_ptr<image>           image_ptr;
		typedef res::basic_resource_manager<image> image_manager;

		uint32_t get_format_stride(xng_image_format format);
	}
}