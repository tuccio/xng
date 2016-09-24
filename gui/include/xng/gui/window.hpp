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

			const wchar_t * get_caption(void) const;
			void set_caption(const wchar_t * caption);

			void extract(gui_command_list_inserter & inserter, const style & style) const override;

		protected:

			window(const window &) = default;

			void on_rectangle_update(const rectangle & oldRectangle, const rectangle & newRectangle) override;

		private:

			bool         m_dragging;
			math::int2   m_mouseDelta;
			rectangle    m_captionRectangle;
			std::wstring m_caption;

			void update_rectangles(void);

			friend class gui_manager;
		};
	}
}