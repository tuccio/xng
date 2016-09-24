#pragma once

#include <xng/gui/widget.hpp>
#include <xng/graphics/image.hpp>

namespace xng
{
	namespace gui
	{
		class image_control :
			public widget
		{

		public:

			image_control(gui_manager * manager, widget * parent, const math::int2 & position = math::int2(0), const math::int2 & size = math::int2(0));

			graphics::image_ptr get_image(void) const;
			void set_image(graphics::image_ptr image);

			void fit(void);

			void extract(gui_command_list_inserter & inserter, const style & style) const override;

		protected:

			image_control(const image_control &) = default;

		private:

			graphics::image_ptr m_image;

		};
	}
}