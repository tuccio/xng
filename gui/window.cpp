#include <xng/gui/window.hpp>
#include <xng/gui/vertical_layout.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;
using namespace xng::res;
using namespace xng::graphics;

#define XNG_GUI_WINDOW_BORDER_SIZE 3

window::window(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_WINDOW, position, size),
	m_dragging(false),
	m_mouseDelta(0)
{
	update_rectangles();

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

void window::extract(gui_command_list_inserter & inserter, const style & style) const
{
	rectangle windowBodyRectangle = get_rectangle();
	windowBodyRectangle.top = m_captionRectangle.bottom;

	*inserter++ = make_filled_rectangle_command(m_captionRectangle, style.caption_background_color);
	*inserter++ = make_filled_rectangle_command(windowBodyRectangle, style.window_background_color);

	font_ptr fnt = resource_factory::get_singleton()->create<font>(style.caption_font);

	*inserter++ = make_text_command(
		fnt,
		m_caption.c_str(),
		style.caption_text_color,
		style.caption_text_border_color,
		style.caption_text_border_size,
		style.caption_text_width,
		(uint2)get_rectangle().topLeft + style.caption_text_origin,
		style.caption_text_scale);
}

bool window::on_mouse_key_down(const mouse * mouse, xng_mouse_key key)
{
	if (propagate_key_down(mouse, key))
	{
		if (key == XNG_MOUSE_BUTTON_1)
		{
			int2 x = (int2)mouse->get_position();
			rectangle rect = get_rectangle();

			if (rectangle_contains(rect, x))
			{
				focus();

				if (rectangle_contains(m_captionRectangle, x))
				{
					m_mouseDelta = x - get_position();
					m_dragging   = true;
				}

				return false;
			}
		}

		return true;
	}

	return false;
}

bool window::on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	if (key == XNG_MOUSE_BUTTON_1)
	{
		if (m_dragging)
		{
			m_dragging = false;
			return false;
		}
	}
	
	return propagate_key_up(mouse, key, millis);
}

bool window::on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	if (key == XNG_MOUSE_BUTTON_1)
	{
		if (m_dragging)
		{
			set_position((int2)mouse->get_position() - m_mouseDelta);
			return false;
		}
	}	
	
	return propagate_key_hold(mouse, key, millis);
}

void window::on_rectangle_update(const rectangle & oldRectangle, const rectangle & newRectangle)
{
	update_rectangles();
}


void window::update_rectangles(void)
{
	int32_t h = get_gui_manager()->get_style().caption_height;

	rectangle rect = get_rectangle();

	m_captionRectangle = rect;
	m_captionRectangle.bottom = m_captionRectangle.top + h;

	rectangle clientRect = rect;

	clientRect.top    += h;
	clientRect.bottom -= XNG_GUI_WINDOW_BORDER_SIZE;
	clientRect.right  -= XNG_GUI_WINDOW_BORDER_SIZE;;
	clientRect.left   += XNG_GUI_WINDOW_BORDER_SIZE;

	set_client_rectangle(clientRect);
}

const wchar_t * window::get_caption(void) const
{
	return m_caption.c_str();
}

void window::set_caption(const wchar_t * caption)
{
	m_caption = caption;
}