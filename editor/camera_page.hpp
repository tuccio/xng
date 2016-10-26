#pragma once

#include "wx.hpp"

#include <xng/graphics.hpp>

namespace xng
{
	namespace editor
	{
		class camera_page :
			public wxWindow
		{

		public:

			camera_page(graphics::scene_graph_camera * camera, wxWindow * parent);

		private:

			graphics::scene_graph_camera * m_camera;

		};
	}
}