#include <xng/gui/window_body.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

window_body::window_body(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_WINDOW_BODY, position, size) {}

void window_body::render(gui_renderer * renderer, const style & style) const
{
	renderer->render_filled_rectangle(get_rectangle(), style.window_background_color);
	render_children(renderer, style);
}

window_body * window_body::clone(gui_manager * manager, widget * parent) const
{
	window_body * newWindow = xng_new window_body(*this);

	newWindow->set_parent(parent);
	newWindow->set_gui_manager(manager);

	clone_children(manager, newWindow);

	return newWindow;
}