#include "fps_camera_controller.hpp"

using namespace xng::editor;
using namespace xng::graphics;
using namespace xng::math;
using namespace xng::input;

fps_camera_controller::fps_camera_controller(void) :
	m_camera(nullptr),
	m_velocity(0),
	m_acceleration(0),
	m_sensitivity(1.f),
	m_speed(1.f),
	m_centerMouse(false),
	m_centeringMouse(false),
	m_holdToRotate(true),
	m_invertX(false),
	m_invertY(false),
	m_rotating(false),
	m_strafingForward(false),
	m_strafingBackward(false),
	m_strafingLeft(false),
	m_strafingRight(false),
	m_strafingUpward(false),
	m_strafingDownward(false)
{

	m_keyboardBinds = {
		{ XNG_KEYBOARD_W, XNG_CAMERA_ACTION_FORWARD },
		{ XNG_KEYBOARD_A, XNG_CAMERA_ACTION_LEFT },
		{ XNG_KEYBOARD_S, XNG_CAMERA_ACTION_BACKWARD },
		{ XNG_KEYBOARD_D, XNG_CAMERA_ACTION_RIGHT },
		{ XNG_KEYBOARD_E, XNG_CAMERA_ACTION_UP },
		{ XNG_KEYBOARD_Q, XNG_CAMERA_ACTION_DOWN }
	};

	m_mouseBinds = {
		{ XNG_MOUSE_BUTTON_2, XNG_CAMERA_ACTION_ROTATE }
	};

}

fps_camera_controller::fps_camera_controller(scene_graph_camera * camera) :
	fps_camera_controller()
{
	set_camera(camera);
}

void fps_camera_controller::bind_keyboard_key(xng_keyboard_key key, xng_camera_action action)
{
	m_keyboardBinds[key] = action;
}

void fps_camera_controller::unbind_keyboard_key(xng_keyboard_key key)
{
	auto it = m_keyboardBinds.find(key);

	if (it != m_keyboardBinds.end())
	{
		m_keyboardBinds.erase(it);
	}
}

void fps_camera_controller::bind_mouse_key(xng_mouse_key key, xng_camera_action action)
{
	m_mouseBinds[key] = action;
}

void fps_camera_controller::unbind_mouse_key(xng_mouse_key key)
{
	auto it = m_mouseBinds.find(key);

	if (it != m_mouseBinds.end())
	{
		m_mouseBinds.erase(it);

	}
}

void fps_camera_controller::clear_keyboard_binds(void)
{
	m_keyboardBinds.clear();
}

void fps_camera_controller::clear_mouse_binds(void)
{
	m_mouseBinds.clear();
}

void fps_camera_controller::on_resize(UINT width, UINT height)
{
	m_screenSize = float2(width, height);
	m_screenCenter = float2(width / 2, height / 2);
}

void fps_camera_controller::on_update(float dt)
{
	if (m_camera)
	{
		const float3 f = m_camera->get_camera()->forward();
		const float3 r = m_camera->get_camera()->right();
		const float3 u = m_camera->get_camera()->up();

		if (m_strafingForward)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position + (m_speed.z * dt) * f;
			m_camera->set_local_translation(newPosition);
		}

		if (m_strafingBackward)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position - (m_speed.z * dt) * f;
			m_camera->set_local_translation(newPosition);
		}

		if (m_strafingLeft)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position - (m_speed.x * dt) * r;
			m_camera->set_local_translation(newPosition);
		}

		if (m_strafingRight)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position + (m_speed.x * dt) * r;
			m_camera->set_local_translation(newPosition);
		}

		if (m_strafingUpward)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position + (m_speed.x * dt) * u;
			m_camera->set_local_translation(newPosition);
		}

		if (m_strafingDownward)
		{
			float3 position = m_camera->get_local_translation();
			float3 newPosition = position - (m_speed.x * dt) * u;
			m_camera->set_local_translation(newPosition);
		}
	}
}

void fps_camera_controller::handle_action_key(xng_camera_action action, bool pressed)
{
	switch (action)
	{
	case XNG_CAMERA_ACTION_FORWARD:
		m_strafingForward = pressed;
		break;
	case XNG_CAMERA_ACTION_BACKWARD:
		m_strafingBackward = pressed;
		break;
	case XNG_CAMERA_ACTION_LEFT:
		m_strafingLeft = pressed;
		break;
	case XNG_CAMERA_ACTION_RIGHT:
		m_strafingRight = pressed;
		break;
	case XNG_CAMERA_ACTION_UP:
		m_strafingUpward = pressed;
		break;
	case XNG_CAMERA_ACTION_DOWN:
		m_strafingDownward = pressed;
		break;
	case XNG_CAMERA_ACTION_ROTATE:
		m_rotating = pressed;
		break;
	}
}

bool fps_camera_controller::on_keyboard_key_down(const keyboard * keyboard, xng_keyboard_key key)
{
	if (m_camera)
	{
		auto it = m_keyboardBinds.find(key);

		if (it != m_keyboardBinds.end())
		{
			handle_action_key(it->second, true);
			return true;
		}
	}

	return false;
}

bool fps_camera_controller::on_keyboard_key_up(const keyboard * keyboard, xng_keyboard_key key, uint32_t millis)
{
	if (m_camera)
	{
		auto it = m_keyboardBinds.find(key);

		if (it != m_keyboardBinds.end())
		{
			handle_action_key(it->second, false);
			return true;
		}
	}
	
	return false;
}

bool fps_camera_controller::on_mouse_wheel(const mouse * mouse, int32_t delta)
{
	m_speed = m_speed * (delta * .2f + 1.f);
	return true;
}

bool fps_camera_controller::on_mouse_key_down(const mouse * mouse, xng_mouse_key key)
{
	if (m_camera)
	{
		auto it = m_mouseBinds.find(key);

		if (it != m_mouseBinds.end())
		{
			handle_action_key(it->second, true);
			return true;
		}
	}

	return false;
}

bool fps_camera_controller::on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
	if (m_camera)
	{
		auto it = m_mouseBinds.find(key);

		if (it != m_mouseBinds.end())
		{
			handle_action_key(it->second, false);
			return true;
		}
	}

	return false;
}


bool fps_camera_controller::on_mouse_move(const mouse * mouse, const uint2 & to)
{
	if (m_rotating)
	{
		float2 from = m_lastMousePosition;;
		float2 distance = (float2)to - from;
		float2 t = distance / float2(m_screenSize.x, m_screenSize.y);
		float2 delta(std::atan(t.x), std::atan(t.y));

		quaternion o = m_camera->get_local_rotation();

		quaternion r1(float3(0, -1, 0), m_invertX ? -delta.x : delta.x);
		quaternion r2(float3(-1, 0, 0), m_invertY ? -delta.y : delta.y);

		quaternion newOrientation = normalize(r1 * o * r2);

		m_camera->set_local_rotation(newOrientation);
	}

	m_lastMousePosition = (float2)to;

	return !m_rotating;
}

void fps_camera_controller::set_camera(scene_graph_camera * camera)
{
	m_camera = camera;
}

scene_graph_camera * fps_camera_controller::get_camera(void) const
{
	return m_camera;
}

void fps_camera_controller::set_screen_size(const float2 & size)
{
	m_screenSize = size;
}