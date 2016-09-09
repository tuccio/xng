#pragma once

#include <xng/gui/widget.hpp>
#include <xng/gui/caption.hpp>
#include <xng/gui/rectangle.hpp>
#include <xng/math.hpp>

namespace xng
{
	namespace gui
	{
		class window_body :
			public widget
		{

		public:

			window_body(gui_manager * manager, widget * parent, const math::int2 & position, const math::int2 & size);

			window_body * clone(gui_manager * manager, widget * parent) const override;

		protected:

			window_body(const window_body &) = default;
			void render(gui_renderer * renderer, const style & styleManager) const override;

		};
	}
}