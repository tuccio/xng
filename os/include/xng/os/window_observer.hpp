#pragma once

#include <xng/os/forward_decl.hpp>
#include <glm/glm.hpp>

namespace xng
{
	namespace os
	{
		struct window_observer
		{
			virtual void on_create(window * wnd) {}
			virtual void on_destroy(window * wnd) {}

			virtual void on_resize(window * wnd, const glm::ivec2 & windowSize, const glm::ivec2 & clientSize) {}

		};
	}
}