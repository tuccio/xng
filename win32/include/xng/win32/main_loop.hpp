#pragma once

#include <xng/os/forward_decl.hpp>

namespace xng
{
	namespace win32
	{

		class main_loop
		{

		public:

			main_loop(void);
			
			void set_idle_callback(os::main_loop_idle_cb idle);

			void run(void);
			void quit(void);

		private:

			os::main_loop_idle_cb m_idleCB;

		};

	}
}