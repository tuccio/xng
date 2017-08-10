#include <xng/engine.hpp>
#include <xng/vk/core/api_context.hpp>

#include <memory>

namespace xng
{
	namespace vk
	{
		class deferred_module :
			public engine::render_module
		{

		public:

			static const char *          module_name;
			static const char *          module_description;
			static const xng_module_type module_type;

			bool init(os::native_window * window_body) override;
			void shutdown(void) override;
			bool is_initialized(void) const override;

			void render(const graphics::extracted_scene & extractedScene, const gui::gui_command_list & guiCommandList) override;
			graphics::api_context * get_api_context(void) override;

		private:

			std::unique_ptr<api_context> m_context;

		};
	}
}