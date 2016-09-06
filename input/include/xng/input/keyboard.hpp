#pragma once

#include <xng/os/timestamp.hpp>
#include <xng/core.hpp>

#include <xng/input/vkeys.hpp>

namespace xng
{
	namespace input
	{

		struct keyboard_observer
		{
			virtual bool on_keyboard_key_down(xng_keyboard_key key) { return true; }
			virtual bool on_keyboard_key_up(xng_keyboard_key key, uint32_t millis) { return true;  }
		};

		class keyboard :
			public core::observable<keyboard_observer>
		{

		public:

			keyboard(void);

			bool is_pressed(xng_keyboard_key) const;
			void clear(void);

		private:

			os::high_resolution_timestamp m_key[XNG_KEYBOARD_MAX];

			void press(xng_keyboard_key key, os::high_resolution_timestamp t);
			void release(xng_keyboard_key key, os::high_resolution_timestamp t);
			void release_all(os::high_resolution_timestamp t);

			friend class input_handler;

		};
	}
}