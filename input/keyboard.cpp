#include <xng/input/keyboard.hpp>

using namespace xng::input;
using namespace xng::os;

keyboard::keyboard(void)
{
	memset(m_key, 0, sizeof(m_key));
}

bool keyboard::is_pressed(xng_keyboard_key key) const
{
	return m_key[key] != 0;
}

void keyboard::press(xng_keyboard_key key, high_resolution_timestamp t)
{
	m_key[key] = t;
	notify(&keyboard_observer::on_keyboard_key_down, key);
}

void keyboard::release(xng_keyboard_key key, high_resolution_timestamp t)
{
	uint32_t dt = to_milliseconds<uint32_t>(t - m_key[key]);
	m_key[key] = 0;
	notify(&keyboard_observer::on_keyboard_key_up, key, dt);
}

void keyboard::release_all(high_resolution_timestamp t)
{
	for (xng_keyboard_key key = XNG_KEYBOARD_UNKNOWN;
		key < XNG_KEYBOARD_MAX;
		key = static_cast<xng_keyboard_key>(key + 1))
	{
		if (is_pressed(key))
		{
			release(key, t);
		}
	}
}

void keyboard::clear(void)
{
	memset(m_key, 0, sizeof(m_key));
	clear_observers();
}