#include <xng/gui/ghost_window.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

ghost_window::ghost_window(const int2 & position, const int2 & size) :
	widget(nullptr, nullptr, XNG_GUI_GHOST_WINDOW, position, size) {}

ghost_window * ghost_window::clone(gui_manager * manager, widget * parent) const
{
	ghost_window * newWindow = xng_new ghost_window(*this);
	clone_children(manager, newWindow);
	return newWindow;
}