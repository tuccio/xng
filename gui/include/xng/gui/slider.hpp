#pragma once

#include <xng/gui/widget.hpp>

namespace xng
{
	namespace gui
	{
		class slider :
			public widget
		{

		public:

			slider(gui_manager * manager, widget * parent, const math::int2 & position = math::int2(0), const math::int2 & size = math::int2(0));

			slider * clone(gui_manager * manager, widget * parent) const override;

			float get_percentage(void) const;
			void set_percentage(float p);

		protected:

			slider(const slider &) = default;
			void render(gui_renderer * renderer, const style & styleManager) const override;

			bool on_mouse_key_down(const input::mouse * mouse, xng_mouse_key key) override;
			bool on_mouse_key_up(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;
			bool on_mouse_key_hold(const input::mouse * mouse, xng_mouse_key key, uint32_t millis) override;

			void on_rectangle_update(const rectangle & oldRectangle, const rectangle & newRectangle) override;

		private:

			bool       m_dragging;
			math::int2 m_mouseDelta;
			float      m_percentage;
			rectangle  m_sliderRectangle;
			rectangle  m_sliderBarRectangle;

			void update_rectangles(void);

		};
	}
}