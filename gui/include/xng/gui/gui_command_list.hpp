#pragma once

#include <xng/gui/rectangle.hpp>
#include <xng/graphics.hpp>
#include <string>
#include <iterator>

enum xng_gui_render_command_type
{
	XNG_GUI_COMMAND_FILLED_RECTANGLE,
	XNG_GUI_COMMAND_TEXTURED_RECTANGLE,
	XNG_GUI_COMMAND_TEXT
};

namespace xng
{
	namespace gui
	{
		class widget;

		struct gui_command
		{
			xng_gui_render_command_type type;

			union
			{
				rectangle   rect;
				math::uint2 position;
			};

			union
			{
				struct
				{
					math::float2 uv0;
					math::float2 uv1;
				};
				
				struct
				{
					float        width;
					uint32_t     border_size;
					math::float4 border_color;
					float        scale;
				};
			};

			math::float4        color;
			std::wstring        text;
			
			graphics::image_ptr image;
			graphics::font_ptr  font;
		};

		typedef std::vector<gui_command>                    gui_command_list;
		typedef std::back_insert_iterator<gui_command_list> gui_command_list_inserter;

		gui_command make_filled_rectangle_command(const rectangle & rect, const math::float4 & color);
		gui_command make_textured_rectangle_command(const rectangle & rect, const math::float2 & uv0, const math::float2 & uv1, const graphics::image_ptr & img);
		gui_command make_text_command(const graphics::font_ptr & font, const wchar_t * text, const math::float4 & color, const math::float4 & borderColor, uint32_t borderSize, float width, const math::uint2 & position, float scale);

	}
}