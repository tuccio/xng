#pragma once

#include <xng/graphics/api_context.hpp>

namespace xng
{
	namespace vk
	{
		class api_context :
			public graphics::api_context
		{

		public:

			bool init(os::native_window_handle handle, xng_api_version version = XNG_API_DEFAULT, bool debug = false) override;
			void shutdown(void) override;

			bool is_initialized(void) const override;
			void frame_complete(void) override;

			void set_vsync(bool vsync) override;
			bool get_vsync(void) const override;
		};
	}
}