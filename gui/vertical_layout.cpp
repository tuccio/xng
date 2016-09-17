#include <xng/gui/vertical_layout.hpp>

using namespace xng::gui;
using namespace xng::math;

vertical_layout::vertical_layout(void) :
	m_weightSum(0) {}

void vertical_layout::apply(void)
{
	rectangle client = m_owner->get_client_rectangle();
	rectangle rect   = m_owner->get_rectangle();

	int2 size(rectangle_width(client), rectangle_height(client));

	int32_t heightMinusFixed = size.y;

	float invWeight = m_weightSum != 0 ? (1.f / m_weightSum) : 0.f;

	for (const layout_entry & entry : m_entries)
	{
		if (entry.proportion == 0)
		{
			uint32_t widgetHeight = entry.widget->get_size().y;
			heightMinusFixed = heightMinusFixed - widgetHeight - 2 * entry.padding;
		}
		else
		{
			heightMinusFixed = heightMinusFixed - 2 * entry.padding;
		}
	}

	heightMinusFixed = max(0, heightMinusFixed);
	int2 offset = client.topLeft - rect.topLeft;

	for (const layout_entry & entry : m_entries)
	{

		int2 oldSize = entry.widget->get_size();
		int2 newSize;

		newSize.x = size.x - entry.padding * 2;
		newSize.y = entry.proportion != 0 ? entry.proportion * invWeight * heightMinusFixed : oldSize.y;

		entry.widget->set_size(newSize);
		entry.widget->set_position(int2(entry.padding) + offset);

		offset.y += newSize.y + 2 * entry.padding;
	}
}

void vertical_layout::add(widget * widget, uint32_t flags, uint32_t proportion, uint32_t padding)
{
	m_weightSum += proportion;
	m_entries.push_back(layout_entry{ widget, flags, proportion, padding });
}
