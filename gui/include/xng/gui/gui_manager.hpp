#pragma once

#include <xng/gui/widget.hpp>
#include <xng/gui/window.hpp>
#include <xng/gui/style.hpp>
#include <xng/gui/gui_events.hpp>
#include <xng/gui/gui_command_list.hpp>
#include <xng/core/event_handler.hpp>

#include <memory>
#include <list>
#include <unordered_set>

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

			void set_style(const style & style);
			const style & get_style(void) const;

			void update(float dt);

			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t) override;
			bool on_mouse_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t) override;

			gui_command_list extract(void) const;

		private:

			typedef core::event_handler<const widget, xng_gui_events> gui_event_handler;

			style               m_style;

			widget            * m_focus;
			gui_event_handler * m_eventHandler;

			std::list<window*> m_windowStack;

			std::unordered_set<widget*> m_destroyQueue;

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

			void enqueue_destruction(widget * widget);

			bool has_pending_objects(void) const;
			void destroy_pending_objects(void);

		};
	}
}