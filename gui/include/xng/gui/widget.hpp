#pragma once

#include <xng/gui/gui_renderer.hpp>
#include <xng/gui/style.hpp>
#include <xng/gui/rectangle.hpp>
#include <xng/gui/layout.hpp>
#include <xng/gui/gui_events.hpp>
#include <xng/gui/gui_command_list.hpp>

#include <xng/math.hpp>
#include <xng/input.hpp>

#include <vector>

namespace xng
{
	namespace gui
	{
		class gui_manager;

		class widget :
			protected input::mouse_observer
		{

		public:

			typedef std::vector<widget*> children_list;
			typedef children_list::const_iterator children_iterator;

			widget(gui_manager * manager, widget * parent, xng_gui_widget type, const math::int2 & position, const math::int2 & size);
			virtual ~widget(void);

			gui_manager * get_gui_manager(void);
			widget * get_parent(void);
			const widget * get_parent(void) const;

			children_iterator begin(void) const;
			children_iterator end(void) const;

			math::int2 get_position(void) const;
			math::int2 get_absolute_position(void) const;
			void set_position(const math::int2 & position);

			math::int2 get_size(void) const;
			void set_size(const math::int2 & size);

			rectangle get_rectangle(void) const;
			void set_rectangle(const rectangle & r);

			rectangle get_client_rectangle(void) const;

			void destroy(void);

			void show(void);
			void hide(void);

			bool is_visible(void) const;

			void set_layout(layout * layout);
			void apply_layout(void);

			void focus(void);

			bool is_window(void) const;

			bool is_relative(void) const;

			template <xng_gui_events evt, typename F>
			void bind(F && function)
			{
				get_gui_manager()->get_event_handler()->bind<evt>(this, std::forward<F>(function));
			}

			virtual void extract(gui_command_list_inserter & inserter, const style & style) const;

		protected:

			widget(const widget & widget);

			void set_client_rectangle(const rectangle & rect);

			virtual void on_rectangle_update(const rectangle & oldRectangle, const rectangle & newRectangle);

			void update_children_rectangles(const rectangle & oldRectangle, const rectangle & newRectangle) const;

			xng_gui_widget get_widget_type(void) const;
			xng_gui_status get_widget_status(void) const;

			void set_widget_status(xng_gui_status);

			void set_parent(widget * parent);
			void set_gui_manager(gui_manager * manager);

			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;

			bool propagate_key_down(const input::mouse * mouse, xng_mouse_key key);
			bool propagate_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t millis);
			bool propagate_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t millis);

			void set_relative(bool relative);

			math::int2 make_absolute(const math::int2 & x) const;
			math::int2 make_relative(const math::int2 & x) const;

			template <xng_gui_events evt, typename ... Args>
			void notify(Args && ... args)
			{
				get_gui_manager()->get_event_handler()->notify_event<evt, Args...>(this, std::forward<Args>(args) ...);
			}

		private:

			gui_manager    * m_manager;
			widget         * m_parent;
			layout         * m_layout;
			bool             m_visible;
			bool             m_relative;
			xng_gui_status   m_status;
			xng_gui_widget   m_type;

			rectangle        m_rectangle;
			rectangle        m_clientRectangle;

			children_list    m_children;

			friend class gui_manager;

			template <typename OutputIterator>
			void collect_visible_widgets(OutputIterator & it)
			{
				std::copy_if(m_children.begin(), m_children.end(), it,
					[](widget * w)
				{
					return !w->is_window() && w->is_visible();
				});

				for (widget * child : m_children)
				{
					if (!child->is_window())
					{
						child->collect_visible_widgets(it);
					}
				}
			}

		};
	}
}