#include <xng/gui/gui_manager.hpp>

#include <algorithm>
#include <map>
#include <iterator>
#include <vector>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;

gui_manager::gui_manager(void) :
    m_focus(nullptr),
    m_hover(nullptr)
{
    m_style        = make_default_style();
    m_eventHandler = xng_new gui_event_handler();
}

gui_manager::~gui_manager(void)
{
    for (window * wnd : m_windowStack)
    {
        wnd->set_parent(nullptr);
        wnd->destroy();
    }

    while (has_pending_objects())
    {
        destroy_pending_objects();
    }

    if (m_eventHandler)
    {
        xng_delete m_eventHandler;
    }
}

void gui_manager::set_style(const xng::gui::style & style)
{
    m_style = style;
}

const style & gui_manager::get_style(void) const
{
    return m_style;
}

style & gui_manager::style(void)
{
    return m_style;
}

gui_command_list gui_manager::extract(void) const
{
    gui_command_list commandList;
    gui_command_list_inserter commandListInserter(commandList);

    // Sort the widgets (put the modal widget and its window on top, sort following the stack order)
    // Highest z is on top, default top window has z=0 by default, modal window has z>0
    std::unordered_map<const window*, int> windowOrder;
    int z = 0;

    for (window * wnd : m_windowStack)
    {
        windowOrder[wnd] = z--;
    }

    auto compare = [&](const window * a, const window * b)
    {
        return windowOrder[a] < windowOrder[b];
    };

    std::map<window*, std::vector<widget*>, decltype(compare)> visibleWidgets(compare);

    for (window * wnd : m_windowStack)
    {
        if (wnd->is_visible())
        {
            std::vector<widget*> widgets = { wnd };
            wnd->collect_visible_widgets(std::back_inserter(widgets));
            visibleWidgets.emplace(wnd, std::move(widgets));
        }
    }

    for (auto & p : visibleWidgets)
    {
        for (widget * w : p.second)
        {
            w->extract(commandListInserter, get_style());
        }
    }

    return commandList;
}

void gui_manager::update(float dt)
{
    destroy_pending_objects();
}

void gui_manager::enqueue_destruction(widget * widget)
{
    m_destroyQueue.insert(widget);
}

bool gui_manager::has_pending_objects(void) const
{
    return !m_destroyQueue.empty();
}

void gui_manager::destroy_pending_objects(void)
{
    std::unordered_set<widget*> destroyQueue = std::move(m_destroyQueue);

    for (widget * w : destroyQueue)
    {
        xng_delete w;
    }
}

bool gui_manager::on_mouse_key_down(const mouse * mouse, xng_mouse_key key)
{
    for (window * wnd : m_windowStack)
    {
        if (!wnd->on_mouse_key_down(mouse, key))
        {
            return false;
        }
    }

    return true;
}

bool gui_manager::on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
    for (window * wnd : m_windowStack)
    {
        if (!wnd->on_mouse_key_up(mouse, key, millis))
        {
            return false;
        }
    }

    return true;
}

bool gui_manager::on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis)
{
    for (window * wnd : m_windowStack)
    {
        if (!wnd->on_mouse_key_hold(mouse, key, millis))
        {
            return false;
        }
    }

    return true;
}

bool gui_manager::on_mouse_move(const mouse * mouse, const uint2 & position)
{
    for (window * wnd : m_windowStack)
    {
        if (!wnd->on_mouse_move(mouse, position))
        {
            return false;
        }
    }

    set_hover(nullptr);

    return true;
}

void gui_manager::register_widget(widget * widget)
{
}

void gui_manager::unregister_widget(widget * widget)
{
}

void gui_manager::register_window(window * wnd)
{
    m_windowStack.push_back(wnd);
}

void gui_manager::unregister_window(window * wnd)
{
    std::remove(m_windowStack.begin(), m_windowStack.end(), wnd);
}

void gui_manager::set_focus(widget * child)
{
    if (child)
    {
        window * wnd = find_first_window(child);

        if (wnd)
        {
            move_on_top(wnd);
        }
    }
    
    m_focus = child;
}

void gui_manager::set_hover(widget * wgt)
{
    if (m_hover)
    {
        m_hover->m_status = XNG_GUI_STATUS_DEFAULT;
    }

    if (wgt)
    {
        wgt->m_status = XNG_GUI_STATUS_HOVER;
    }

    m_hover = wgt;
}

void gui_manager::move_on_top(window * wnd)
{
    auto it = std::find(m_windowStack.begin(), m_windowStack.end(), wnd);

    if (it != m_windowStack.begin())
    {
        m_windowStack.splice(m_windowStack.begin(), m_windowStack, it, std::next(it));
    }
}

window * gui_manager::find_first_window(widget * w)
{
    while (w)
    {
        if (w->is_window())
        {
            return static_cast<window*>(w);
        }
        else
        {
            w = w->get_parent();
        }
    }

    return nullptr;
}

gui_manager::gui_event_handler * gui_manager::get_event_handler(void)
{
    return m_eventHandler;
}
