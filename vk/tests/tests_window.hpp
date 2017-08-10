#pragma once

#include <xng/os.hpp>

#include "wx.hpp"

namespace xng
{
	namespace vk
	{
		namespace test
		{
			class tests_window
			{

			public:

				tests_window(os::native_window * window);
				~tests_window(void);

				void update(float dt);

			private:

				void create_menu(void);

				os::native_window * m_window;
				wxFrame           * m_main;

				wxWindow * m_selectedObjectPage;

			};
		}
	}
}