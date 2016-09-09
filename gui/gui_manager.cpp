#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;

gui_manager::gui_manager(void) :
	m_renderer(nullptr),
	m_size(1280, 720),
	m_focus(nullptr)
{
	m_style = make_default_style();
	m_root  = xng_new ghost_window(int2(0), (int2)m_size);
}

gui_manager::~gui_manager(void)
{
	xng_delete m_root;
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
	m_root->set_size((int2)m_size);
}

void gui_manager::render(void)
{
	assert(m_renderer && "A renderer is needed to render the GUI.");

	m_renderer->render_begin(m_size);

	for (auto it = m_windowStack.rbegin(); it != m_windowStack.rend(); ++it)
	{
		(*it)->render(get_renderer(), get_style());
	}

	m_renderer->render_end();
}

gui_manager * gui_manager::clone(void) const
{
	gui_manager * newGUI = xng_new gui_manager(*this);
	newGUI->m_root = newGUI->m_root->clone(newGUI, nullptr);
	return newGUI;
}

void gui_manager::register_widget(widget * widget)
{
	if (!widget->get_parent())
	{
		m_root->m_children.push_back(widget);
	}
}

void gui_manager::unregister_widget(widget * widget)
{
	if (!widget->get_parent())
	{
		std::remove(m_root->m_children.begin(), m_root->m_children.end(), widget);
	}
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