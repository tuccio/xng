#pragma once

#include <type_traits>
#include <xng/os/win32_headers.hpp>
#include <xng/os/timestamp.hpp>

namespace xng
{
	namespace os
	{
		template <typename T = double>
		class high_resolution_stopwatch
		{
		public:

			high_resolution_stopwatch(void)
			{
				m_last = timestamp();
			}

			XNG_INLINE T get_elapsed_seconds(void)
			{
				high_resolution_timestamp now = timestamp();
				high_resolution_timestamp dt  = now - m_last;
				m_last = now;
				return to_seconds<T>(dt);
			}

			XNG_INLINE high_resolution_timestamp get_last_timestamp(void) const
			{
				return m_last;
			}

		private:

			high_resolution_timestamp m_last;
		};

	}
}