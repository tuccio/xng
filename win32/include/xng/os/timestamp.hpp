#pragma once

#include <xng/os/win32_headers.hpp>
#include <type_traits>

namespace xng
{
	namespace os
	{
		namespace detail
		{
			struct frequency_initializer
			{
				frequency_initializer(void)
				{
					QueryPerformanceFrequency(&frequency);
				}

				LARGE_INTEGER frequency;
			};

			extern const frequency_initializer g_performanceCounterFrequency;
		}

		typedef LONGLONG high_resolution_timestamp;

		XNG_INLINE high_resolution_timestamp timestamp(void)
		{
			LARGE_INTEGER timestamp;
			QueryPerformanceCounter(&timestamp);
			return timestamp.QuadPart;
		}

		template <typename T>
		XNG_INLINE std::enable_if_t<std::is_floating_point<T>::value, T> to_seconds(high_resolution_timestamp time)
		{
			high_resolution_timestamp micro = (time * 1000000) / detail::g_performanceCounterFrequency.frequency.QuadPart;
			return micro * T(.000001);
		}

		template <typename T>
		XNG_INLINE std::enable_if_t<std::is_integral<T>::value, T> to_milliseconds(high_resolution_timestamp time)
		{
			return (time * 1000) / detail::g_performanceCounterFrequency.frequency.QuadPart;
		}
	}
}