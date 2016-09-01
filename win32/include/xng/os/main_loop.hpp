#pragma once

#include <functional>
#include <atomic>

namespace xng
{
	namespace os
	{
		typedef std::function<void(void)> main_loop_idle_cb;

		class main_loop
		{

		public:

			main_loop(void);

			void set_idle_callback(main_loop_idle_cb idle);

			void run(void);
			bool is_running(void) const;
			void quit(void);

		private:

			main_loop_idle_cb m_idleCB;
			std::atomic<bool> m_running;

		};

	}
}