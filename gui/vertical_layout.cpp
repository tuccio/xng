#include <xng/gui/vertical_layout.hpp>

using namespace xng::gui;
using namespace xng::math;

vertical_layout::vertical_layout(void) :
	m_weightSum(0) {}

void vertical_layout::apply(void)
{
	int2 size = m_owner->get_size();

	int32_t heightMinusFixed = size.y;

	float invWeight = m_weightSum != 0 ? (1.f / m_weightSum) : 0.f;

	for (const layout_entry & entry : m_entries)
	{
		if (entry.proportion == 0)
		{
			uint32_t widgetHeight = entry.widget->get_size().y;
			heightMinusFixed = heightMinusFixed - widgetHeight;
		}
	}

	heightMinusFixed = max(0, heightMinusFixed);
	uint32_t y = 0;

	for (const layout_entry & entry : m_entries)
	{
		int2 oldSize = entry.widget->get_size();
		int2 newSize;

		newSize.x = size.x;
		newSize.y = entry.proportion != 0 ? entry.proportion * invWeight * heightMinusFixed : oldSize.y;

		entry.widget->set_size(newSize);
		entry.widget->set_position(int2(0, y));

		y += newSize.y;
	}
}

void vertical_layout::add(widget * widget, uint32_t flags, uint32_t proportion)
{
	m_weightSum += proportion;
	m_entries.push_back(layout_entry{ widget, flags, proportion });
}
