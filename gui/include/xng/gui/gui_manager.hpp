#pragma once

#include <xng/gui/widget.hpp>
#include <xng/gui/gui_renderer.hpp>
#include <xng/gui/window.hpp>
#include <xng/gui/style.hpp>
#include <xng/gui/gui_events.hpp>
#include <xng/core/event_handler.hpp>

#include <memory>
#include <list>

namespace xng
{
	namespace gui
	{
		
		class gui_manager :
			public input::mouse_observer
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

			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t) override;
			bool on_mouse_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t) override;

		private:

			typedef core::event_handler<const widget, xng_gui_events> gui_event_handler;

			gui_renderer * m_renderer;
			style          m_style;
			math::uint2    m_size;

			widget       * m_focus;

			std::list<window*> m_windowStack;
			gui_event_handler * m_eventHandler;

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

			gui_event_handler * get_event_handler(void);

		};
	}
}