#pragma once

#include <xng/os/forward_decl.hpp>
#include <xng/math.hpp>

namespace xng
{
	namespace os
	{
		struct window_observer
		{
			virtual void on_create(window * wnd) {}
			virtual void on_destroy(window * wnd) {}

			virtual void on_resize(window * wnd, const math::uint2 & windowSize, const math::uint2 & clientSize) {}
		};
	}
}