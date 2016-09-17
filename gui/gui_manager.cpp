#include <xng/gui/gui_manager.hpp>

#include <algorithm>
#include <map>
#include <iterator>
#include <vector>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;

gui_manager::gui_manager(void) :
	m_renderer(nullptr),
	m_size(1280, 720),
	m_focus(nullptr),
	m_shallow(false)
{
	m_style        = make_default_style();
	m_eventHandler = xng_new gui_event_handler();
}

gui_manager::~gui_manager(void)
{
	if (!m_shallow)
	{
		for (window * wnd : m_windowStack)
		{
			wnd->destroy();
		}

		destroy_pending_objects();

		if (m_eventHandler)
		{
			xng_delete m_eventHandler;
		}
	}
}

void gui_manager::set_renderer(gui_renderer * renderer)
{
	m_renderer = renderer;
}

gui_renderer * gui_manager::get_renderer(void) const
{
	return m_renderer;
}

void gui_manager::set_style(const style & style)
{
	m_style = style;
}

const style & gui_manager::get_style(void) const
{
	return m_style;
}

const uint2 & gui_manager::get_size(void) const
{
	return m_size;
}

void gui_manager::set_size(const uint2 & size)
{
	m_size = size;
}

void gui_manager::render(void)
{
	assert(m_renderer && "A renderer is needed to render the GUI.");

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

	m_renderer->render_begin(m_size);

	for (auto & p : visibleWidgets)
	{
		for (widget * w : p.second)
		{
			w->render(get_renderer(), get_style());
		}
	}

	m_renderer->render_end();
}

void gui_manager::update(float dt)
{
	destroy_pending_objects();
}

void gui_manager::enqueue_destruction(widget * widget)
{
	m_destroyQueue.insert(widget);
}

void gui_manager::destroy_pending_objects(void)
{
	for (widget * w : m_destroyQueue)
	{
		xng_delete w;
	}

	m_destroyQueue.clear();
}

gui_manager * gui_manager::clone(void) const
{
	gui_manager * newGUI = xng_new gui_manager(*this);

	newGUI->m_eventHandler = nullptr;
	newGUI->m_shallow      = true;

	return newGUI;
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
