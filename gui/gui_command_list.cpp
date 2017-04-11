#include <xng/gui/gui_command_list.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::graphics;

gui_command xng::gui::make_filled_rectangle_command(const rectangle & rect, const float4 & color)
{
    gui_command cmd;

    cmd.type  = XNG_GUI_COMMAND_FILLED_RECTANGLE;

    cmd.rect  = rect;
    cmd.color = color;

    return cmd;
}

gui_command xng::gui::make_textured_rectangle_command(const rectangle & rect, const float2 & uv0, const float2 & uv1, const image_ptr & img)
{
    gui_command cmd;

    cmd.type  = XNG_GUI_COMMAND_TEXTURED_RECTANGLE;

    cmd.image = img;
    cmd.rect  = rect;
    cmd.uv0   = uv0;
    cmd.uv1   = uv1;

    return cmd;
}

gui_command xng::gui::make_text_command(const font_ptr & font, const wchar_t * text, const float4 & color, const float4 & borderColor, uint32_t borderSize, float thinness, const uint2 & position, float scale)
{
    gui_command cmd;

    cmd.type         = XNG_GUI_COMMAND_TEXT;

    cmd.font         = font;
    cmd.text         = text;
    cmd.color        = color;
    cmd.border_color = borderColor;
    cmd.border_size  = borderSize;
    cmd.thinness     = thinness;
    cmd.position     = position;
    cmd.scale        = scale;

    return cmd;
}