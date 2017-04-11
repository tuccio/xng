#include <xng/os/main_loop.hpp>

#include <Windows.h>

using namespace xng;
using namespace xng::os;

main_loop::main_loop(void) : m_running(false) {}

void main_loop::set_idle_callback(std::function<void()> idle)
{
    m_idleCB = idle;
}

void main_loop::run(void)
{
    if (m_idleCB)
    {
        MSG msg;

        m_running = true;

        while (m_running)
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            m_idleCB();
        }
    }
    else
    {
        MSG msg;

        m_running = true;

        while (m_running)
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}

bool main_loop::is_running(void) const
{
    return m_running;
}

void main_loop::quit(void)
{
    m_running = false;
}