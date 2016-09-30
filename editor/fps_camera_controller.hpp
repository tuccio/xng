#pragma once

#include <xng/math.hpp>
#include <xng/input.hpp>
#include <xng/graphics.hpp>

enum xng_camera_action
{
	XNG_CAMERA_ACTION_FORWARD,
	XNG_CAMERA_ACTION_BACKWARD,
	XNG_CAMERA_ACTION_LEFT,
	XNG_CAMERA_ACTION_RIGHT,
	XNG_CAMERA_ACTION_UP,
	XNG_CAMERA_ACTION_DOWN,
	XNG_CAMERA_ACTION_ROTATE
};

namespace xng
{
	namespace editor
	{
		class fps_camera_controller :
			public input::keyboard_observer,
			public input::mouse_observer
		{

		public:

			fps_camera_controller(void);
			fps_camera_controller(graphics::scene_graph_camera * camera);

			void bind_keyboard_key(xng_keyboard_key key, xng_camera_action action);
			void unbind_keyboard_key(xng_keyboard_key key);

			void bind_mouse_key(xng_mouse_key key, xng_camera_action action);
			void unbind_mouse_key(xng_mouse_key key);

			void clear_keyboard_binds(void);
			void clear_mouse_binds(void);

			void on_resize(UINT width, UINT height);
			void on_update(float dt);

			bool on_keyboard_key_down(const input::keyboard * keyboard, xng_keyboard_key key) override;
			bool on_keyboard_key_up(const input::keyboard * keyboard, xng_keyboard_key key, uint32_t millis) override;

			bool on_mouse_wheel(const input::mouse * mouse, int32_t delta) override;
			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;
			bool on_mouse_move(const input::mouse * mouse, const math::uint2 & position) override;

			void set_camera(graphics::scene_graph_camera * camera);
			graphics::scene_graph_camera * get_camera(void) const;

			void set_screen_size(const math::float2 & size);

		private:


			math::float3 m_acceleration;
			math::float3 m_velocity;

			math::float3 m_speed;

			math::float2 m_screenSize;
			math::float2 m_screenCenter;

			math::float2 m_lastMousePosition;

			float  m_sensitivity;

			bool   m_centerMouse;
			bool   m_centeringMouse;
			bool   m_holdToRotate;

			bool   m_invertX;
			bool   m_invertY;

			bool   m_rotating;

			bool   m_strafingForward;
			bool   m_strafingBackward;
			bool   m_strafingLeft;
			bool   m_strafingRight;
			bool   m_strafingUpward;
			bool   m_strafingDownward;

			graphics::scene_graph_camera * m_camera;

			std::unordered_map<xng_keyboard_key, xng_camera_action> m_keyboardBinds;
			std::unordered_map<UINT, xng_camera_action>             m_mouseBinds;
			
			void handle_action_key(xng_camera_action action, bool pressed);

		};
	}
}