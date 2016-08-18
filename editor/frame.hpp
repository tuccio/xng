#pragma once

#include "wx.hpp"
#include "render_panel.hpp"

namespace xng
{
	namespace editor
	{
		class frame :
			public wxFrame
		{

		public:

			frame(const wxString & caption, wxPoint position, wxSize size);
			~frame(void);

			render_panel * get_render_panel(void) { return m_renderPanel; }

			bool load_aui_conf(const char * filename);
			bool save_aui_conf(const char * filename);

		private:

			wxAuiManager   m_manager;
			render_panel * m_renderPanel;

		};
	}
}