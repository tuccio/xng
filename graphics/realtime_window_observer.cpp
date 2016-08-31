#include <xng/graphics/realtime_window_observer.hpp>

using namespace xng::math;
using namespace xng::os;
using namespace xng::graphics;

void realtime_window_observer::on_resize(native_window * wnd, const uint2 & windowSize, const uint2 & clientSize)
{
	if (m_configuration)
	{
		m_configuration->set_render_resolution(clientSize);
	}

	if (m_context)
	{
		m_context->on_resize(clientSize.x, clientSize.y);
	}
}