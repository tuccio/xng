#include <xng/input/input_handler.hpp>
#include <iterator>

using namespace xng::input;
using namespace xng::os;

void input_handler::on_keyboard_key_down(xng_keyboard_key key, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_KEYBOARD;
	event.time   = time;

	event.keyboardEvent.key = key;
	event.keyboardEvent.type = XNG_KEYBOARD_EVENT_TYPE_KEYDOWN;

	m_queue.push(event);
}

void input_handler::on_keyboard_key_up(xng_keyboard_key key, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_KEYBOARD;
	event.time   = time;

	event.keyboardEvent.key = key;
	event.keyboardEvent.type = XNG_KEYBOARD_EVENT_TYPE_KEYUP;

	m_queue.push(event);
}

void input_handler::on_mouse_key_down(xng_mouse_key key, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_MOUSE;
	event.time   = time;

	event.mouseEvent.key = key;
	event.mouseEvent.type = XNG_MOUSE_EVENT_TYPE_KEYDOWN;

	m_queue.push(event);
}

void input_handler::on_mouse_key_up(xng_mouse_key key, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_MOUSE;
	event.time   = time;

	event.mouseEvent.key = key;
	event.mouseEvent.type = XNG_MOUSE_EVENT_TYPE_KEYUP;

	m_queue.push(event);
}

void input_handler::on_mouse_move(const xng::math::uint2 & position, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_MOUSE;
	event.time   = time;

	event.mouseEvent.position = position;
	event.mouseEvent.type     = XNG_MOUSE_EVENT_TYPE_MOVE;

	m_queue.push(event);
}

void input_handler::on_mouse_wheel(int32_t delta, high_resolution_timestamp time)
{
	input_event event;

	event.source = XNG_INPUT_EVENT_SOURCE_MOUSE;
	event.time   = time;

	event.mouseEvent.wheel = delta;
	event.mouseEvent.type  = XNG_MOUSE_EVENT_TYPE_WHEEL;

	m_queue.push(event);
}

void input_handler::on_focus_change(high_resolution_timestamp time)
{
	m_keyboard.release_all(time);
	m_mouse.release_all(time);
	m_mouse.reset_wheel();
}

void input_handler::dispatch(high_resolution_timestamp time)
{
	m_mouse.reset_wheel();
	m_events.clear();
	m_queue.pop(time, std::back_inserter(m_events));

	for (const input_event & event : m_events)
	{
		switch (event.source)
		{

		case XNG_INPUT_EVENT_SOURCE_KEYBOARD:
			if (event.keyboardEvent.type == XNG_KEYBOARD_EVENT_TYPE_KEYDOWN)
			{
				m_keyboard.press(event.keyboardEvent.key, event.time);
			}
			else
			{
				m_keyboard.release(event.keyboardEvent.key, event.time);
			}
			break;

		case XNG_INPUT_EVENT_SOURCE_MOUSE:
			if (event.mouseEvent.type == XNG_MOUSE_EVENT_TYPE_KEYDOWN)
			{
				m_mouse.press(event.mouseEvent.key, event.time);
			}
			else if (event.mouseEvent.type == XNG_MOUSE_EVENT_TYPE_KEYUP)
			{
				m_mouse.release(event.mouseEvent.key, event.time);
			}
			else if (event.mouseEvent.type == XNG_MOUSE_EVENT_TYPE_WHEEL)
			{
				m_mouse.wheel(event.mouseEvent.wheel);
			}
			else if (event.mouseEvent.type == XNG_MOUSE_EVENT_TYPE_MOVE)
			{
				m_mouse.move(event.mouseEvent.position);
			}
			break;

		}
	}
}

void input_handler::clear(void)
{
	m_queue.clear();
	m_keyboard.clear();
	m_mouse.clear();
}

const keyboard & input_handler::keyboard(void) const
{
	return m_keyboard;
}

const mouse & input_handler::mouse(void) const
{
	return m_mouse;
}