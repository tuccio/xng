#pragma once

#include <xng/gui/widget.hpp>
#include <xng/gui/caption.hpp>

namespace xng
{
	namespace gui
	{
		class caption :
			public widget
		{

		public:

			caption(gui_manager * manager, widget * parent, const math::int2 & position, const math::int2 & size);

			caption * clone(gui_manager * manager, widget * parent) const override;

		protected:

			caption(const caption &) = default;
			void render(gui_renderer * renderer, const style & styleManager) const override;

		};
	}
}