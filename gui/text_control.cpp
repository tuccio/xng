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
    const text_control_style * textControlStyle = get_text_control_style();
    font_ptr fnt = resource_factory::get_singleton()->create<font>(textControlStyle->text.font.c_str());

    if (fnt && fnt->load())
    {
        uint2 origin(textControlStyle->padding);

        origin.y += fnt->get_point_size() * textControlStyle->text.scale;

        *inserter++ = make_filled_rectangle_command(get_rectangle(),
            m_editable ? textControlStyle->editable_background_color : textControlStyle->background_color);

        *inserter++ = make_text_command(
            fnt,
            m_text.c_str(),
            textControlStyle->text.color,
            textControlStyle->text.border_color,
            textControlStyle->text.border_size,
            textControlStyle->text.thinness,
            (uint2)get_rectangle().topLeft + origin,
            textControlStyle->text.scale);
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
    const text_control_style * style = get_text_control_style();
    font_ptr fnt = resource_factory::get_singleton()->create<font>(style->text.font.c_str());

    if (fnt && fnt->load())
    {
        uint32_t size = fnt->get_point_size();

        uint32_t w = size * style->text.scale * m_text.length() + 2 * style->padding;
        uint32_t h = size * 1.25f * style->text.scale + 2 * style->padding;

        set_size(int2(w, h));
    }
}

const text_control_style * text_control::get_text_control_style(void) const
{
    return m_style ? m_style.get() : &get_gui_manager()->get_style().text_control;
}

void text_control::set_text_control_style(const text_control_style & style)
{
    m_style = std::make_unique<text_control_style>(style);
}