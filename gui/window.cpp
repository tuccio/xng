#include <xng/gui/window.hpp>
#include <xng/gui/vertical_layout.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

window::window(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_WINDOW, position, size)
{
	m_caption = xng_new caption(manager, this, position, size);
	m_body    = xng_new window_body(manager, this, position, size);

	vertical_layout * layout = xng_new vertical_layout();

	layout->add(m_caption, XNG_LAYOUT_EXPAND, 0);
	layout->add(m_body, XNG_LAYOUT_EXPAND, 1);

	set_layout(layout);
	fit();

	if (manager)
	{
		manager->register_window(this);
	}
}

window::~window(void)
{
	if (auto manager = get_gui_manager())
	{
		manager->unregister_window(this);
	}
}

window * window::clone(gui_manager * manager, widget * parent) const
{
	window * newWindow = xng_new window(*this);

	newWindow->set_parent(parent);
	newWindow->set_gui_manager(manager);

	for (widget * child : *this)
	{
		widget * newChild = child->clone(manager, newWindow);

		if (child == m_body)
		{
			newWindow->m_body = static_cast<window_body*>(child);
		}
	}

	return newWindow;
}

bool window::is_window(void) const
{
	return true;
}