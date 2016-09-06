#include <xng/graphics/realtime_window_observer.hpp>

using namespace xng::math;
using namespace xng::os;
using namespace xng::graphics;

realtime_window_observer & realtime_window_observer::operator= (const realtime_window_observer & rhs)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_configuration = rhs.m_configuration;
	m_context       = rhs.m_context;
	m_resize        = false;
	return *this;
}

void realtime_window_observer::on_resize(native_window * wnd, const uint2 & windowSize, const uint2 & clientSize)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_configuration)
	{
		m_configuration->set_render_resolution(clientSize);
	}

	// We will pass the resize information to the rendering thread so we can safely resize the buffers

	m_resize     = true;
	m_renderSize = clientSize;
}

void realtime_window_observer::update(void)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Since this is called in the rendering thread, we can safely resize

	if (m_resize && m_context)
	{
		m_context->on_resize(m_renderSize.x, m_renderSize.y);
		m_resize = false;
	}
}