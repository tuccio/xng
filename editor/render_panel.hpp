#pragma once

#include "wx.hpp"

#include <xng/graphics/api_context.hpp>
#include <memory>

namespace xng
{
	namespace editor
	{
		class render_panel :
			public wxPanel
		{

		public:

			render_panel(
				wxWindow * parent,
				wxWindowID id = wxID_ANY,
				const wxPoint & pos = wxDefaultPosition,
				const wxSize & size = wxDefaultSize,
				long style = wxTAB_TRAVERSAL,
				const wxString & name = wxPanelNameStr);

		private:

			void on_resize(wxSizeEvent & event);
			void on_keyboard_key_down(wxKeyEvent & event);
			void on_keyboard_key_up(wxKeyEvent & event);
			void on_mouse_event(wxMouseEvent & event);

			wxDECLARE_EVENT_TABLE();

		};
	}
}