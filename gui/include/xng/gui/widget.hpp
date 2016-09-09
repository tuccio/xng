#pragma once

#include <xng/gui/gui_renderer.hpp>
#include <xng/gui/style.hpp>
#include <xng/gui/rectangle.hpp>
#include <xng/gui/layout.hpp>

#include <xng/math.hpp>

#include <vector>

namespace xng
{
	namespace gui
	{
		class gui_manager;

		class widget
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

			const math::int2 & get_position(void) const;
			void set_position(const math::int2 & position);

			const math::int2 & get_size(void) const;
			void set_size(const math::int2 & size);

			rectangle get_rectangle(void) const;

			void show(void);
			void hide(void);

			bool is_visible(void) const;

			void fit(void);

			void focus(void);

			virtual bool is_window(void) const;

			virtual widget * clone(gui_manager * manager, widget * parent) const = 0;

		protected:

			widget(const widget & widget);
			virtual void render(gui_renderer * renderer, const style & style) const;

			void clone_children(gui_manager * manager, widget * parent) const;
			void render_children(gui_renderer * renderer, const style & style) const;

			xng_gui_widget get_widget_type(void) const;
			xng_gui_status get_widget_status(void) const;

			void set_widget_status(xng_gui_status);

			void set_parent(widget * parent);
			void set_gui_manager(gui_manager * manager);

			void set_layout(layout * layout);

		private:

			gui_manager    * m_manager;
			widget         * m_parent;
			layout         * m_layout;
			bool             m_visible;
			xng_gui_status   m_status;
			xng_gui_widget   m_type;

			math::int2 m_position;
			math::int2 m_size;

			children_list m_children;

			friend class gui_manager;

		};
	}
}