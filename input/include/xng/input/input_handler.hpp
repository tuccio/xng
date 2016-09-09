#pragma once

#include <xng/input/input_queue.hpp>
#include <xng/input/keyboard.hpp>
#include <xng/input/mouse.hpp>

#include <xng/os/timestamp.hpp>

namespace xng
{
	namespace input
	{
		class input_handler
		{

		public:

			void on_keyboard_key_down(xng_keyboard_key key, os::high_resolution_timestamp time);
			void on_keyboard_key_up(xng_keyboard_key key, os::high_resolution_timestamp time);

			void on_mouse_key_down(xng_mouse_key key, os::high_resolution_timestamp time);
			void on_mouse_key_up(xng_mouse_key key, os::high_resolution_timestamp time);

			void on_mouse_move(const math::uint2 & position, os::high_resolution_timestamp time);
			void on_mouse_wheel(int32_t delta, os::high_resolution_timestamp time);

			void on_focus_change(os::high_resolution_timestamp time);

			void dispatch(os::high_resolution_timestamp time);
			void clear(void);

			const keyboard & keyboard(void) const;
			const mouse & mouse(void) const;

		private:

			input::input_queue m_queue;
			input::keyboard    m_keyboard;
			input::mouse       m_mouse;

			input_event_vector m_events;

		};
	}
}