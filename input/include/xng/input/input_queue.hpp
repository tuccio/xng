#pragma once

#include <xng/input/vkeys.hpp>
#include <xng/os/high_resolution_timer.hpp>

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

namespace xng
{
	namespace input
	{

		struct keyboard_event
		{
			xng_keyboard_key        key;
			xng_keyboard_event_type type;

		};

		struct input_event
		{
			os::high_resolution_timestamp time;
			xng_input_event_source source;

			union
			{
				keyboard_event keyboardEvent;
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

				if (it != m_events.end())
				{
					auto end = std::next(it);
					std::copy(m_events.begin(), end, outIt);
					m_events.erase(m_events.begin(), end);
				}
			}

			void clear(void);

		private:

			std::vector<input_event> m_events;

			mutable std::mutex m_mutex;

		};
	}
}