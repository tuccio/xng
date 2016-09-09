#include <xng/os.hpp>

#include "wx.hpp"

namespace xng
{
	namespace editor
	{
		struct render_panel :
			public wxWindow
		{

			render_panel(os::native_window * window_body, wxWindow * parent, wxWindowID id);
			~render_panel(void);

			void on_resize(wxSizeEvent & event);

			wxDECLARE_EVENT_TABLE();

		private:

			os::native_window * m_window;

		};
	}
}