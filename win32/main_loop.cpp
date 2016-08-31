#include <xng/os/main_loop.hpp>

#include <Windows.h>

using namespace xng;
using namespace xng::os;

main_loop::main_loop(void) {}

void main_loop::set_idle_callback(std::function<void()> idle)
{
	m_idleCB = idle;
}

void main_loop::run(void)
{
	MSG msg;

	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		if (m_idleCB)
		{
			m_idleCB();
		}
		
	}
}

void main_loop::quit(void)
{
	PostQuitMessage(0);
}