#include <xng/gui/layout.hpp>

using namespace xng::gui;

layout::layout(void) :
	m_owner(nullptr)
{
}

void layout::set_owner(widget * owner)
{
	m_owner = owner;
}