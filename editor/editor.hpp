#pragma once

#include <xng/os.hpp>
#include "wx.hpp"

#include <wx/aui/aui.h>

namespace xng
{
	namespace editor
	{
		class editor
		{

		public:

			editor(os::native_window * window_body);
			~editor(void);

			void update(void);

			wxFrame * get_main_window(void);

		private:

			wxFrame * m_editor;
			std::unique_ptr<wxAuiManager> m_auiManager;

		};
	}
}