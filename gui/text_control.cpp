#include <xng/gui/text_control.hpp>
#include <xng/gui/gui_manager.hpp>

#include <xng/graphics.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::graphics;
using namespace xng::res;

text_control::text_control(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_TEXT_CONTROL, position, size),
	m_editable(false) {}

void text_control::extract(gui_command_list_inserter & inserter, const style & style) const
{
	font_ptr fnt = resource_factory::get_singleton()->create<font>(style.text_control_font.c_str());

	if (fnt && fnt->load())
	{
		uint2 origin(style.text_control_padding);

		origin.y += fnt->get_point_size() * style.text_control_text_scale;

		*inserter++ = make_filled_rectangle_command(get_rectangle(),
			m_editable ? style.text_control_editable_background_color : style.text_control_background_color);

		*inserter++ = make_text_command(
			fnt,
			m_text.c_str(),
			style.text_control_text_color,
			style.text_control_text_border_color,
			style.text_control_text_border_size,
			style.text_control_text_thinness,
			(uint2)get_rectangle().topLeft + origin,
			style.text_control_text_scale);

	}
}

const wchar_t * text_control::get_text(void) const
{
	return m_text.c_str();
}

bool text_control::is_editable(void) const
{
	return m_editable;
}

void text_control::set_editable(bool editable)
{
	m_editable = editable;
}

void text_control::fit(void)
{
	const style & style = get_gui_manager()->style();
	font_ptr fnt = resource_factory::get_singleton()->create<font>(style.caption_font.c_str());

	if (fnt && fnt->load())
	{
		uint32_t size = fnt->get_point_size();

		uint32_t w = size * style.text_control_text_scale * m_text.length() + 2 * style.text_control_padding;
		uint32_t h = size * 1.5f * style.text_control_text_scale + 2 * style.text_control_padding;

		set_size(int2(w, h));
	}
}