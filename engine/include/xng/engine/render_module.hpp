#pragma once

#include <xng/engine/module.hpp>

#include <xng/graphics.hpp>

namespace xng
{
	namespace engine
	{
		class render_module :
			public module
		{

		public:

			virtual bool init(os::native_window * window) = 0;
			virtual void shutdown(void) = 0;

			virtual bool is_initialized(void) const = 0;

			virtual void render(graphics::scene * scene, const graphics::camera * camera) = 0;
			virtual graphics::api_context * get_api_context(void) = 0;

			XNG_INLINE graphics::renderer_configuration & configuration(void)
			{
				return m_configuration;
			}

		protected:

			graphics::renderer_configuration m_configuration;

		};
	}
}