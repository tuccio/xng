#include "render_panel.hpp"
#include "globals.hpp"

using namespace xng::editor;

wxBEGIN_EVENT_TABLE(render_panel, wxPanel)
	EVT_SIZE(render_panel::on_resize)
	EVT_KEY_DOWN(render_panel::on_keyboard_key_down)
	EVT_KEY_UP(render_panel::on_keyboard_key_up)
	EVT_MOUSE_EVENTS(render_panel::on_mouse_event)
wxEND_EVENT_TABLE()

render_panel::render_panel(
	wxWindow * parent,
	wxWindowID id,
	const wxPoint & pos,
	const wxSize & size,
	long style,
	const wxString & name) :
	wxPanel(parent, id, pos, size, style, name) {}

void render_panel::on_resize(wxSizeEvent & event)
{
	if (g_renderer)
	{
		wxSize size = event.GetSize();
		g_renderer->get_configuration()->set_render_resolution(glm::ivec2(size.x, size.y));
		g_apiContext->on_resize(size.x, size.y);
	}
}

void render_panel::on_keyboard_key_down(wxKeyEvent & event)
{

}

void render_panel::on_keyboard_key_up(wxKeyEvent & event)
{

}

void render_panel::on_mouse_event(wxMouseEvent & event)
{

}