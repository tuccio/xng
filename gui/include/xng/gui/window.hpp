#pragma once

#include <xng/gui/widget.hpp>
#include <string>

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

			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;
			bool on_mouse_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;

			XNG_INLINE void set_relative(bool relative)
			{
				widget::set_relative(relative);
			}

			const char * get_caption(void) const;
			void set_caption(const char * caption);

		protected:

			window(const window &) = default;

			window * clone(gui_manager * manager, widget * parent) const override;
			void render(gui_renderer * renderer, const style & style) const override;

			void on_reposition(const math::int2 & oldPosition, const math::int2 & newPosition) override;
			void on_resize(const math::int2 & oldSize, const math::int2 & newSize) override;

		private:

			bool        m_dragging;
			math::int2  m_mouseDelta;
			rectangle   m_captionRectangle;
			std::string m_caption;

			void update_rectangles(void);

			friend class gui_manager;
		};
	}
}