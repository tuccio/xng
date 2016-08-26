#pragma once

#include <xng/os/forward_decl.hpp>
#include <xng/graphics/api_version.hpp>

#include <cstdint>

namespace xng
{
	namespace graphics
	{

		struct api_context
		{
			api_context(void) {}
			virtual ~api_context(void) {}

			virtual bool init(os::native_window_handle handle, xng_api_version version = XNG_API_DEFAULT, bool debug = false) = 0;
			virtual void shutdown(void) = 0;

			virtual bool is_initialized(void) const = 0;

			virtual void use(void) {}
			virtual void dispose(void) {}

			virtual void frame_start(void) {}
			virtual void frame_complete(void) = 0;

			virtual void on_resize(uint32_t width, uint32_t height) {}

			virtual void set_vsync(bool vsync) = 0;
			virtual bool get_vsync(void) const = 0;
			
		private:

			uint64_t m_frame;

		};
	}
}