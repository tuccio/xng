#pragma once

#include <xng/math.hpp>

namespace xng
{
	namespace os
	{
		class native_window;

		struct native_window_observer
		{
			virtual void on_create(native_window * wnd);
			virtual void on_destroy(native_window * wnd);

			virtual void on_resize(native_window * wnd, const math::uint2 & windowSize, const math::uint2 & clientSize);
		};
	}
}