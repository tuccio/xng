#pragma once

#include <xng/engine/module.hpp>

#include <xng/graphics.hpp>
#include <xng/gui.hpp>

namespace xng
{
	namespace engine
	{
		class render_module :
			public module
		{

		public:

			virtual bool init(os::native_window * window_body) = 0;
			virtual void shutdown(void) = 0;

			virtual bool is_initialized(void) const = 0;

			virtual void render(const graphics::extracted_scene & extractedScene, const gui::gui_command_list & guiCommandList) = 0;

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