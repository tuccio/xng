#include <xng/input/keyboard.hpp>

using namespace xng::input;
using namespace xng::os;

keyboard::keyboard(void) {}

bool keyboard::is_pressed(xng_keyboard_key key) const
{
	auto it = m_keys.find(key);
	return it != m_keys.end();
}

void keyboard::press(xng_keyboard_key key, high_resolution_timestamp t)
{
	if (!is_pressed(key))
	{
		m_keys[key] = t;
		notify(&keyboard_observer::on_keyboard_key_down, this, key);
	}
}

void keyboard::release(xng_keyboard_key key, high_resolution_timestamp t)
{
	auto it = m_keys.find(key);

	if (it != m_keys.end())
	{
		uint32_t dt = to_milliseconds<float>(t - it->second);
		m_keys.erase(it);
		notify(&keyboard_observer::on_keyboard_key_up, this, key, dt);
	}
}

void keyboard::release_all(high_resolution_timestamp t)
{
	for (auto it = m_keys.begin(); it != m_keys.end();)
	{
		uint32_t dt = to_milliseconds<float>(t - it->second);
		xng_keyboard_key key = it->first;
		it = m_keys.erase(it);
		notify(&keyboard_observer::on_keyboard_key_up, this, key, dt);
	}
}

void keyboard::clear(void)
{
	clear_observers();
	m_keys.clear();
}