#pragma once

#include <xng/gui/window_body.hpp>
#include <xng/gui/caption.hpp>

namespace xng
{
	namespace gui
	{
		class window :
			public widget
		{

		public:

			window(gui_manager * manager, widget * parent, const math::int2 & position, const math::int2 & size);
			~window(void);

			window * clone(gui_manager * manager, widget * parent) const override;

			bool is_window(void) const;

		protected:

			window(const window &) = default;

		private:

			caption     * m_caption;
			window_body * m_body;

		};
	}
}