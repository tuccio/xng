#include <xng/gui/widget.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

widget::widget(gui_manager * manager, widget * parent, xng_gui_widget type, const int2 & position, const int2 & size)
{
	m_type     = type;
	m_status   = XNG_GUI_STATUS_DEFAULT;
	m_parent   = parent;
	m_manager  = manager;
	m_size     = size;
	m_position = position;
	m_layout   = nullptr;
	m_visible  = false;

	set_parent(parent);
	set_gui_manager(manager);
}

widget::widget(const widget & widget)
{
	// Copy constructor used in clone

	m_type     = widget.m_type;
	m_status   = widget.m_status;
	m_parent   = nullptr;
	m_manager  = nullptr;
	m_size     = widget.m_size;
	m_position = widget.m_position;
	m_layout   = nullptr;
	m_visible  = widget.m_visible;
}

widget::~widget(void)
{
	if (m_manager)
	{
		m_manager->unregister_widget(this);
	}

	set_layout(nullptr);

	for (widget * child : m_children)
	{
		xng_delete child;
	}
}

gui_manager * widget::get_gui_manager(void)
{
	return m_manager;
}

void widget::set_gui_manager(gui_manager * manager)
{
	m_manager = manager;

	if (m_manager)
	{
		m_manager->register_widget(this);
	}
}

const widget * widget::get_parent(void) const
{
	return m_parent;
}

widget * widget::get_parent(void)
{
	return m_parent;
}

void widget::set_parent(widget * parent)
{
	m_parent = parent;

	if (parent)
	{
		parent->m_children.push_back(this);
	}
}

widget::children_iterator widget::begin(void) const
{
	return m_children.begin();
}

widget::children_iterator widget::end(void) const
{
	return m_children.end();
}

bool widget::is_visible(void) const
{
	return m_visible;
}

void widget::fit(void)
{
	if (m_layout)
	{
		m_layout->apply();
	}
}

void widget::focus(void)
{
	m_manager->set_focus(this);
}

void widget::show(void)
{
	m_visible = true;

	for (widget * child : m_children)
	{
		child->show();
	}
}

void widget::set_layout(layout * layout)
{
	if (m_layout)
	{
		xng_delete layout;
	}

	if (layout)
	{
		layout->set_owner(this);
		m_layout = layout;
	}
}

void widget::hide(void)
{
	m_visible = false;

	for (widget * child : m_children)
	{
		child->hide();
	}
}

const int2 & widget::get_position(void) const
{
	return m_position;
}

void widget::set_position(const int2 & position)
{
	m_position = position;
}

const int2 & widget::get_size(void) const
{
	return m_size;
}

void widget::set_size(const int2 & size)
{
	m_size = size;
	fit();
}

rectangle widget::get_rectangle(void) const
{
	int2 base(0);

	if (auto parent = get_parent())
	{
		base = parent->get_position();
	}

	rectangle rect;
	rect.topLeft     = base + m_position;;
	rect.bottomRight = rect.topLeft + m_size;
	return rect;
}

xng_gui_widget widget::get_widget_type(void) const
{
	return m_type;
}

xng_gui_status widget::get_widget_status(void) const
{
	return m_status;
}

void widget::set_widget_status(xng_gui_status status)
{
	m_status = status;
}

void widget::clone_children(gui_manager * manager, widget * parent) const
{
	for (widget * child : *this)
	{
		child->clone(manager, parent);
	}
}

void widget::render(gui_renderer * renderer, const style & style) const
{
	render_children(renderer, style);
}

void widget::render_children(gui_renderer * renderer, const style & style) const
{
	for (widget * child : *this)
	{
		if (!child->is_window())
		{
			child->render(renderer, style);
		}
	}
}

bool widget::is_window(void) const
{
	return false;
}