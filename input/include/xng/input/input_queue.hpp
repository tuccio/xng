#pragma once

#include <xng/input/vkeys.hpp>
#include <xng/os/high_resolution_timer.hpp>

#include <xng/math.hpp>

#include <algorithm>
#include <mutex>
#include <vector>

enum xng_input_event_source
{
	XNG_INPUT_EVENT_SOURCE_KEYBOARD,
	XNG_INPUT_EVENT_SOURCE_MOUSE
};

enum xng_keyboard_event_type
{
	XNG_KEYBOARD_EVENT_TYPE_KEYDOWN,
	XNG_KEYBOARD_EVENT_TYPE_KEYUP
};

enum xng_mouse_event_type
{
	XNG_MOUSE_EVENT_TYPE_KEYDOWN,
	XNG_MOUSE_EVENT_TYPE_KEYUP,
	XNG_MOUSE_EVENT_TYPE_MOVE,
	XNG_MOUSE_EVENT_TYPE_WHEEL
};

namespace xng
{
	namespace input
	{

		struct keyboard_event
		{
			xng_keyboard_key        key;
			xng_keyboard_event_type type;
		};

		struct mouse_event
		{
			union
			{
				xng_mouse_key key;
				int32_t       wheel;
				math::uint2   position;
			};

			xng_mouse_event_type type;
		};

		struct input_event
		{
			os::high_resolution_timestamp time;
			xng_input_event_source source;

			union
			{
				keyboard_event keyboardEvent;
				mouse_event    mouseEvent;
			};
		};

		typedef std::vector<input_event> input_event_vector;

		class input_queue
		{

		public:

			void push(const input_event & event);

			template <typename OutputIterator>
			void pop(os::high_resolution_timestamp time, OutputIterator outIt)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto it = std::upper_bound(m_events.begin(), m_events.end(), input_event{ time },
					[](const input_event & a, const input_event & b)
				{
					return a.time < b.time;
				});

				auto end = it != m_events.end() ? std::next(it) : it;
				std::copy(m_events.begin(), end, outIt);
				m_events.erase(m_events.begin(), end);
			}

			void clear(void);

		private:

			std::vector<input_event> m_events;

			mutable std::mutex m_mutex;

		};
	}
}