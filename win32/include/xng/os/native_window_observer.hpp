#pragma once

#include <xng/math.hpp>
#include <xng/input/vkeys.hpp>

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

			virtual void on_focus(native_window * wnd);
			virtual void on_unfocus(native_window * wnd);

			virtual void on_keyboard_key_down(native_window * wnd, xng_keyboard_key key);
			virtual void on_keyboard_key_up(native_window * wnd, xng_keyboard_key key);

			virtual void on_mouse_key_down(native_window * wnd, xng_mouse_key key);
			virtual void on_mouse_key_up(native_window * wnd, xng_mouse_key key);
			virtual void on_mouse_move(native_window * wnd, const math::uint2 & position);
			virtual void on_mouse_wheel(native_window * wnd, int32_t delta);
		};
	}
}