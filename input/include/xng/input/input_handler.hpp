#pragma once

#include <xng/input/input_queue.hpp>
#include <xng/input/keyboard.hpp>

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

			void on_focus_change(os::high_resolution_timestamp time);

			void dispatch(os::high_resolution_timestamp time);
			void clear(void);

			const keyboard & keyboard(void) const;

		private:

			input::input_queue m_queue;
			input::keyboard    m_keyboard;

			input_event_vector m_events;

		};
	}
}