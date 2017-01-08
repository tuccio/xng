#pragma once

#include "wx.hpp"

#include <xng/graphics.hpp>

namespace xng
{
	namespace editor
	{
		class light_page :
			public wxWindow
		{

		public:

			light_page(graphics::scene_graph_light * light, wxWindow * parent);

		private:

			graphics::scene_graph_camera * m_camera;

		};
	}
}