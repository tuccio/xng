#include <xng/os.hpp>
#include <xng/graphics/renderer_configuration.hpp>
#include <xng/graphics/api_context.hpp>

namespace xng
{
	namespace graphics
	{
		struct realtime_window_observer :
			public os::native_window_observer
		{

		public:

			realtime_window_observer(void) :
				m_configuration(nullptr), m_context(nullptr) {}

			realtime_window_observer(renderer_configuration * configuration, api_context * context) :
				m_configuration(configuration), m_context(context) {}

			realtime_window_observer(const realtime_window_observer &) = default;

			void on_resize(os::native_window * wnd, const math::uint2 & windowSize, const math::uint2 & clientSize) override;

		private:

			renderer_configuration * m_configuration;
			api_context            * m_context;

		};
	}
}