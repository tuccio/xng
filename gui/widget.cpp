#include <xng/gui/widget.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;

widget::widget(gui_manager * manager, widget * parent, xng_gui_widget type, const int2 & position, const int2 & size)
{
	m_type     = type;
	m_status   = XNG_GUI_STATUS_DEFAULT;
	m_parent   = parent;
	m_manager  = manager;
	m_layout   = nullptr;
	m_visible  = false;
	m_relative = true;

	m_rectangle.topLeft     = make_absolute(position);
	m_rectangle.bottomRight = m_rectangle.topLeft + size;

	m_clientRectangle = m_rectangle;

	set_parent(parent);
	set_gui_manager(manager);
}

widget::widget(const widget & widget)
{
	// Copy constructor used in clone

	m_type      = widget.m_type;
	m_status    = widget.m_status;
	m_parent    = nullptr;
	m_manager   = nullptr;
	m_rectangle = widget.m_rectangle;
	m_layout    = nullptr;
	m_visible   = widget.m_visible;
}

widget::~widget(void)
{
	if (m_manager)
	{
		m_manager->unregister_widget(this);
	}

	set_layout(nullptr);
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
	if (m_parent)
	{
		std::remove(m_parent->m_children.begin(), m_parent->m_children.end(), this);
	}

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

void widget::apply_layout(void)
{
	//set_position(m_position);
	//set_size(m_size);

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
		xng_delete m_layout;
	}

	if (layout)
	{
		layout->set_owner(this);
	}

	m_layout = layout;
}

bool widget::on_mouse_key_down(const mouse * mouse, xng_mouse_key key)
{
	if (propagate_key_down(mouse, key))
	{
		if (key == XNG_MOUSE_BUTTON_1 &&
			rectangle_contains(get_rectangle(), (int2)mouse->get_position()))
		{
			focus();
			return false;
		}

		return true;
	}

	return false;
}

bool widget::on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	return propagate_key_hold(mouse, key, millis);
}

bool widget::on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	return propagate_key_up(mouse, key, millis);
}

bool widget::on_mouse_move(const mouse * mouse, const uint2 & position)
{
	return propagate_mouse_move(mouse, position);
}

bool widget::propagate_key_down(const mouse * mouse, xng_mouse_key key)
{
	for (widget * child : *this)
	{
		// Propagate to non window children (windows are notified input in stack order)
		if (!child->is_window() && !child->on_mouse_key_down(mouse, key))
		{
			return false;
		}
	}

	return true;
}

bool widget::propagate_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	for (widget * child : *this)
	{
		// Propagate to non window children (windows are notified input in stack order)
		if (!child->is_window() && !child->on_mouse_key_up(mouse, key, millis))
		{
			return false;
		}
	}

	return true;
}

bool widget::propagate_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	for (widget * child : *this)
	{
		// Propagate to non window children (windows are notified input in stack order)
		if (!child->is_window() && !child->on_mouse_key_hold(mouse, key, millis))
		{
			return false;
		}
	}

	return true;
}

bool widget::propagate_mouse_move(const mouse * mouse, const uint2 & position)
{
	for (widget * child : *this)
	{
		// Propagate to non window children (windows are notified input in stack order)
		if (!child->is_window() && !child->on_mouse_move(mouse, position))
		{
			return false;
		}
	}

	return true;
}

void widget::hide(void)
{
	m_visible = false;

	for (widget * child : m_children)
	{
		child->hide();
	}
}

int2 widget::get_position(void) const
{
	return make_relative(m_rectangle.topLeft);
}

int2 widget::get_absolute_position(void) const
{
	return m_rectangle.topLeft;
}

void widget::set_position(const int2 & position)
{
	int2      delta  = make_absolute(position) - m_rectangle.topLeft;
	rectangle rect   = m_rectangle;

	rect.topLeft     = rect.topLeft + delta;
	rect.bottomRight = rect.bottomRight + delta;
	set_rectangle(rect);
}

int2 widget::get_size(void) const
{
	return rectangle_size(m_rectangle);
}

void widget::set_size(const int2 & size)
{
	rectangle rect = m_rectangle;
	rect.bottomRight = rect.topLeft + size;
	set_rectangle(rect);
}

rectangle widget::get_rectangle(void) const
{
	return m_rectangle;
}

void widget::set_rectangle(const rectangle & newRectangle)
{
	rectangle oldRectangle = m_rectangle;

	m_rectangle = newRectangle;

	// Call virtual update functions
	on_rectangle_update(oldRectangle, newRectangle);
	update_children_rectangles(oldRectangle, newRectangle);
}

int2 widget::make_absolute(const int2 & x) const
{
	int2 base = m_relative && get_parent() ?
		get_parent()->get_position() : int2(0);

	return x + base;
}

int2 widget::make_relative(const int2 & x) const
{
	int2 base = m_relative && get_parent() ?
		get_parent()->get_position() : int2(0);

	return x - base;
}

rectangle widget::get_client_rectangle(void) const
{
	return m_clientRectangle;
}

void widget::destroy(void)
{
	m_manager->enqueue_destruction(this);

	for (widget * w : *this)
	{
		w->destroy();
	}
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

void widget::extract(gui_command_list_inserter & inserter, const style & style) const {}

bool widget::is_window(void) const
{
	return get_widget_type() == XNG_GUI_WINDOW;
}

void widget::set_client_rectangle(const rectangle & rect)
{
	m_clientRectangle = rect;
}

void widget::on_rectangle_update(const rectangle & oldRectangle, const rectangle & newRectangle)
{
	set_client_rectangle(newRectangle);
}

void widget::update_children_rectangles(const rectangle & oldRectangle, const rectangle & newRectangle) const
{
	if (oldRectangle.topLeft != newRectangle.topLeft)
	{
		int2 delta = newRectangle.topLeft - oldRectangle.topLeft;

		for (widget * child : *this)
		{
			if (child->is_relative())
			{
				rectangle rect = child->get_rectangle();
				rect.topLeft = rect.topLeft + delta;
				rect.bottomRight = rect.bottomRight + delta;
				child->set_rectangle(rect);
			}
		}
	}
}

bool widget::is_relative(void) const
{
	return m_relative;
}

void widget::set_relative(bool relative)
{
	m_relative = relative;
}

void widget::hover(void)
{
	get_gui_manager()->set_hover(this);
}

xng_gui_status widget::get_status(void) const
{
	return m_status;
}