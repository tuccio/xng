#include <xng/gui/slider.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;

slider::slider(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_SLIDER, position, size),
	m_dragging(false),
	m_percentage(0),
	m_sliderRectangle(rectangle{}),
	m_sliderBarRectangle(rectangle{})
{
	update_rectangles();

	int2 newSize = max(rectangle_size(m_sliderBarRectangle), rectangle_size(m_sliderRectangle));
	set_size(newSize);
}

void slider::render(gui_renderer * renderer, const style & style) const
{
	rectangle filled = m_sliderBarRectangle;
	rectangle empty  = m_sliderBarRectangle;

	int middle = m_sliderRectangle.right - rectangle_width(m_sliderRectangle) / 2;

	filled.right = middle;
	empty.left   = middle;

	renderer->render_filled_rectangle(filled, style.slider_bar_filled);
	renderer->render_filled_rectangle(empty, style.slider_bar_empty);

	renderer->render_filled_rectangle(m_sliderRectangle, style.slider_color);
	render_children(renderer, style);
}

slider * slider::clone(gui_manager * manager, widget * parent) const
{
	slider * newWidget = xng_new slider(*this);

	newWidget->set_parent(parent);
	newWidget->set_gui_manager(manager);

	clone_children(manager, newWidget);

	return newWidget;
}

void slider::on_reposition(const int2 & oldPosition, const int2 & newPosition)
{
	update_rectangles();
}

void slider::on_resize(const int2 & oldSize, const int2 & newSize)
{
	update_rectangles();
}

void slider::on_parent_reposition(void)
{
	update_rectangles();
}

void slider::on_parent_resize(void)
{
	update_rectangles();
}

bool slider::on_mouse_key_down(const mouse * mouse, xng_mouse_key key)
{
	if (propagate_key_down(mouse, key))
	{
		if (key == XNG_MOUSE_BUTTON_1)
		{
			if (rectangle_contains(m_sliderRectangle, (int2)mouse->get_position()))
			{
				focus();
				m_mouseDelta = (int2)mouse->get_position() - make_relative(m_sliderRectangle.topLeft);
				m_dragging   = true;
				return false;
			}
		}

		return true;
	}

	return false;
}

bool slider::on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	if (m_dragging)
	{
		m_dragging = false;
		return false;
	}
	else
	{
		return propagate_key_up(mouse, key, millis);
	}
}

bool slider::on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	if (m_dragging)
	{
		rectangle rect     = get_rectangle();

		int basePosition   = get_position().x;
		int sliderWidth    = rectangle_width(m_sliderRectangle);
		int length         = rectangle_width(m_sliderBarRectangle);
		int sliderPosition = min<int>(max<int>(rect.left, mouse->get_position().x), rect.left + length) - rect.left;

		m_percentage = sliderPosition / (float) length;
		update_rectangles();

		notify<XNG_GUI_EVENT_SLIDER>(m_percentage);

		return false;
	}
	else
	{
		return propagate_key_hold(mouse, key, millis);
	}
}

float slider::get_percentage(void) const
{
	return m_percentage;
}

void slider::set_percentage(float p)
{
	m_percentage = p;
	update_rectangles();
}

void slider::update_rectangles(void)
{
	const style & style = get_gui_manager()->get_style();

	rectangle rect = get_rectangle();

	int2 basePosition = rect.topLeft;
	int  height       = max(style.slider_bar_height, style.slider_size.y);

	int  sliderWidth  = style.slider_size.x;

	m_sliderBarRectangle.left   = basePosition.x + style.slider_size.x / 2;
	m_sliderBarRectangle.top    = basePosition.y + (height - style.slider_bar_height) / 2;
	m_sliderBarRectangle.right  = m_sliderBarRectangle.left + max<int>(0, get_size().x - style.slider_size.x);
	m_sliderBarRectangle.bottom = m_sliderBarRectangle.top + style.slider_bar_height;

	int2 sliderPosition = {
		(int) (basePosition.x + rectangle_width(m_sliderBarRectangle) * m_percentage),
		basePosition.y + (height - (int)style.slider_size.y) / 2
	};

	m_sliderRectangle.topLeft     = sliderPosition;
	m_sliderRectangle.bottomRight = m_sliderRectangle.topLeft + (int2)style.slider_size;
}