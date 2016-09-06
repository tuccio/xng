#include <xng/input/input_handler.hpp>
#include <iterator>

using namespace xng::input;
using namespace xng::os;

void input_handler::on_keyboard_key_down(xng_keyboard_key key, high_resolution_timestamp time)
{
	if (!m_keyboard.is_pressed(key))
	{
		input_event event;

		event.source = XNG_INPUT_EVENT_SOURCE_KEYBOARD;
		event.time = time;

		event.keyboardEvent.key  = key;
		event.keyboardEvent.type = XNG_KEYBOARD_EVENT_TYPE_KEYDOWN;

		m_queue.push(event);
	}
}

void input_handler::on_keyboard_key_up(xng_keyboard_key key, high_resolution_timestamp time)
{
	if (m_keyboard.is_pressed(key))
	{
		input_event event;

		event.source = XNG_INPUT_EVENT_SOURCE_KEYBOARD;
		event.time = time;

		event.keyboardEvent.key = key;
		event.keyboardEvent.type = XNG_KEYBOARD_EVENT_TYPE_KEYUP;

		m_queue.push(event);
	}
}

void input_handler::on_focus_change(high_resolution_timestamp time)
{
	m_keyboard.release_all(time);
}

void input_handler::dispatch(high_resolution_timestamp time)
{
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

		}
	}
}

void input_handler::clear(void)
{
	m_queue.clear();
	m_keyboard.clear();
}

const keyboard & input_handler::keyboard(void) const
{
	return m_keyboard;
}