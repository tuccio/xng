#include <xng/input/input_queue.hpp>

using namespace xng::input;
using namespace xng::os;

void input_queue::push(const input_event & event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.push_back(event);
}

void input_queue::clear(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.clear();
}