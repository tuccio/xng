#pragma once

#include <xng/gui/widget.hpp>
#include <xng/gui/gui_renderer.hpp>
#include <xng/gui/ghost_window.hpp>
#include <xng/gui/window.hpp>
#include <xng/gui/style.hpp>

#include <memory>
#include <list>

namespace xng
{
	namespace gui
	{
		
		class gui_manager
		{

		public:

			gui_manager(void);

			~gui_manager(void);

			void set_renderer(gui_renderer * renderer);
			gui_renderer * get_renderer(void) const;

			void set_style(const style & style);
			const style & get_style(void) const;

			void render(void);

			void set_size(const math::uint2 & size);
			const math::uint2 & get_size(void) const;

			gui_manager * clone(void) const;

		private:

			gui_renderer * m_renderer;
			style          m_style;
			math::uint2    m_size;

			ghost_window * m_root;
			widget       * m_focus;

			std::list<window*> m_windowStack;

			friend class widget;
			friend class window;

			void register_widget(widget * widget);
			void unregister_widget(widget * widget);

			void register_window(window * wnd);
			void unregister_window(window * wnd);

			void set_focus(widget * widget);

			void move_on_top(window * wnd);
			window * find_first_window(widget * w);

			gui_manager(const gui_manager &) = default;


		};
	}
}