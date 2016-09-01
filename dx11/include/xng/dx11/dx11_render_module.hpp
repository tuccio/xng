#include <xng/engine.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/forward_renderer.hpp>

#include <memory>

namespace xng
{
	namespace dx11
	{
		class dx11_render_module :
			public engine::render_module
		{

		public:

			static const char *          module_name;
			static const char *          module_description;
			static const xng_module_type module_type;

			bool init(os::native_window * window) override;
			void shutdown(void) override;
			bool is_initialized(void) const override;

			void render(graphics::scene * scene) override;
			graphics::api_context * get_api_context(void) override;

		private:

			std::unique_ptr<dx11_api_context> m_context;
			std::unique_ptr<forward_renderer> m_renderer;

			native_window * m_window;

			graphics::realtime_window_observer m_windowObserver;

		};
	}
}