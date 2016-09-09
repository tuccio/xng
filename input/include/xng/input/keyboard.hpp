#pragma once

#include <xng/os/timestamp.hpp>
#include <xng/core.hpp>

#include <xng/input/vkeys.hpp>

#include <unordered_map>

namespace xng
{
	namespace input
	{
		class keyboard;

		struct keyboard_observer
		{
			virtual bool on_keyboard_key_down(const keyboard * keyboard, xng_keyboard_key key) { return true; }
			virtual bool on_keyboard_key_up(const keyboard * keyboard, xng_keyboard_key key, uint32_t millis) { return true;  }
		};

		class keyboard :
			public core::observable<keyboard_observer>
		{

		public:

			keyboard(void);

			bool is_pressed(xng_keyboard_key) const;
			void clear(void);

		private:

			typedef std::unordered_map<xng_keyboard_key, os::high_resolution_timestamp> keyboard_map;

			keyboard_map m_keys;

			void press(xng_keyboard_key key, os::high_resolution_timestamp t);
			void release(xng_keyboard_key key, os::high_resolution_timestamp t);
			void release_all(os::high_resolution_timestamp t);

			friend class input_handler;

		};
	}
}