#include <xng/os.hpp>
#include <xng/graphics/renderer_configuration.hpp>
#include <xng/graphics/api_context.hpp>

#include <mutex>

namespace xng
{
	namespace graphics
	{
		struct realtime_window_observer :
			public os::native_window_observer
		{

		public:

			realtime_window_observer(void) :
				m_configuration(nullptr), m_context(nullptr), m_resize(false) {}

			realtime_window_observer(renderer_configuration * configuration, api_context * context) :
				m_configuration(configuration), m_context(context), m_resize(false) {}

			realtime_window_observer(const realtime_window_observer &) :
				realtime_window_observer(m_configuration, m_context) {}

			realtime_window_observer & operator= (const realtime_window_observer & rhs);

			void on_resize(os::native_window * wnd, const math::uint2 & windowSize, const math::uint2 & clientSize) override;

			void update(render_variables * vars, std::set<xng_render_variable> * updates);

		private:

			renderer_configuration * m_configuration;
			api_context            * m_context;

			bool        m_resize;
			math::uint2 m_renderSize;

			mutable std::mutex m_mutex;

		};
	}
}