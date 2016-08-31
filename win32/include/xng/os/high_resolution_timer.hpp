#pragma once

#include <type_traits>
#include <xng/os/win32_headers.hpp>

namespace xng
{
	namespace os
	{
		template <typename T = double>
		class high_resolution_timer
		{
			static_assert(std::is_floating_point<T>::value, "Only floating point types allowed.");

		public:

			high_resolution_timer(void)
			{
				QueryPerformanceCounter(&m_last);
			}

			T get_elapsed_seconds(void)
			{
				LARGE_INTEGER newTime;
				QueryPerformanceCounter(&newTime);

				LARGE_INTEGER dt, dtMicro;;

				dt.QuadPart = newTime.QuadPart - m_last.QuadPart;
				m_last = newTime;

				dtMicro.QuadPart = (dt.QuadPart * 1000000) / s_frequencyInitializer.frequency.QuadPart;

				return dtMicro.QuadPart * T(.000001);
			}

		private:

			static struct frequency_initializer
			{
				frequency_initializer(void)
				{
					QueryPerformanceFrequency(&frequency);
				}

				LARGE_INTEGER frequency;
			} s_frequencyInitializer;

			LARGE_INTEGER m_last;
		};

		template <typename T>
		typename high_resolution_timer<T>::frequency_initializer high_resolution_timer<T>::s_frequencyInitializer;

	}
}