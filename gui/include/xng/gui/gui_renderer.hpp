#pragma once

#include <xng/res.hpp>
#include <xng/gui/rectangle.hpp>

namespace xng
{
	namespace gui
	{
		class gui_renderer
		{

		public:

			virtual void render_begin(const math::uint2 & size) = 0;
			virtual void render_end(void) = 0;

			virtual void render_filled_rectangle(const rectangle & rect, const math::float4 & color) = 0;

		};
	}
}