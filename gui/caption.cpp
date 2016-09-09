#include <xng/gui/caption.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

caption::caption(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_CAPTION, position, int2(size.x, manager->get_style().caption_height)) {}

void caption::render(gui_renderer * renderer, const style & style) const
{
	renderer->render_filled_rectangle(get_rectangle(), style.caption_background_color);
	render_children(renderer, style);
}

caption * caption::clone(gui_manager * manager, widget * parent) const
{
	caption * newWidget = xng_new caption(*this);

	newWidget->set_parent(parent);
	newWidget->set_gui_manager(manager);

	clone_children(manager, newWidget);

	return newWidget;
}