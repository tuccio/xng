#include <xng/input/mouse.hpp>

using namespace xng::input;
using namespace xng::os;

mouse::mouse(void) :
	m_position(0, 0),
	m_wheelDelta(0) {}

bool mouse::is_pressed(xng_mouse_key key) const
{
	auto it = m_keys.find(key);
	return it != m_keys.end();
}

void mouse::press(xng_mouse_key key, high_resolution_timestamp t)
{
	m_keys[key] = t;
	notify(&mouse_observer::on_mouse_key_down, this, key);
}

void mouse::release(xng_mouse_key key, high_resolution_timestamp t)
{
	auto it = m_keys.find(key);

	if (it != m_keys.end())
	{
		uint32_t dt = to_milliseconds<float>(t - it->second);
		m_keys.erase(it);
		notify(&mouse_observer::on_mouse_key_up, this, key, dt);
	}
}

void mouse::release_all(high_resolution_timestamp t)
{
	for (auto it = m_keys.begin(); it != m_keys.end();)
	{
		uint32_t dt = to_milliseconds<float>(t - it->second);
		xng_mouse_key key = it->first;
		it = m_keys.erase(it);
		notify(&mouse_observer::on_mouse_key_up, this, key, dt);
	}
}

void mouse::notify_held_keys(high_resolution_timestamp t)
{
	for (auto & p : m_keys)
	{
		if (t > p.second)
		{
			notify(&mouse_observer::on_mouse_key_hold, this, p.first, to_milliseconds<float>(t - p.second));
		}
	}
}

void mouse::move(const xng::math::uint2 & position)
{
	m_position = position;
	notify(&mouse_observer::on_mouse_move, this, m_position);
}

void mouse::wheel(int32_t delta)
{
	m_wheelDelta += delta;
	notify(&mouse_observer::on_mouse_wheel, this, delta);
}

void mouse::reset_wheel(void)
{
	m_wheelDelta = 0;
}

void mouse::clear(void)
{
	clear_observers();
	m_keys.clear();
}

const xng::math::uint2 & mouse::get_position(void) const
{
	return m_position;
}

int32_t mouse::get_wheel_delta(void) const
{
	return m_wheelDelta;
}