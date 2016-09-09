#include "render_panel.hpp"

using namespace xng::editor;
using namespace xng::os;

wxBEGIN_EVENT_TABLE(render_panel, wxPanel)
	EVT_SIZE(render_panel::on_resize)
wxEND_EVENT_TABLE()

render_panel::render_panel(native_window * window_body, wxWindow * parent, wxWindowID id) :
	wxWindow(parent, id),
	m_window(window_body)
{
	m_window->set_borderless(true);
	::SetParent(m_window->get_native_handle(), GetHwnd());
	m_window->set_position(math::uint2(0, 0));
}

render_panel::~render_panel(void)
{
	m_window->set_borderless(false);
	::SetParent(m_window->get_native_handle(), NULL);
}

void render_panel::on_resize(wxSizeEvent & event)
{
	wxSize size = event.GetSize();
	m_window->set_window_size(math::uint2(size.x, size.y));
}