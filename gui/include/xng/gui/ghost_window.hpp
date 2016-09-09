#pragma once

#include <xng/gui/widget.hpp>

namespace xng
{
	namespace gui
	{
		class ghost_window :
			public widget
		{

		public:

			ghost_window(const math::int2 & position, const math::int2 & size);

			ghost_window * clone(gui_manager * manager, widget * parent) const override;

		protected:

			ghost_window(const ghost_window &) = default;

		};
	}
}