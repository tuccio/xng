#pragma once

#include <xng/gui/widget_enum.hpp>
#include <xng/math.hpp>

#include <string>

namespace xng
{
	namespace gui
	{
		struct text_style
		{
			std::string  font;
			math::uint2  origin;
			math::float4 color;
			math::float4 border_color;
			uint32_t     border_size;
			float        thinness;
			float        scale;
		};

		struct caption_style
		{
			math::float4 background_color;
			uint32_t     height;
			text_style   text;
		};

		struct window_style
		{
			math::float4  background_color;
			caption_style caption;
		};

		struct slider_style
		{
			math::uint2  size;
			math::float4 color;
			math::float4 hover_color;
			math::float4 bar_filled_color;
			math::float4 bar_empty_color;
			uint32_t     bar_height;
		};

		struct text_control_style
		{
			math::float4 background_color;
			math::float4 editable_background_color;
			uint32_t     padding;
			text_style   text;
		};

		struct style
		{
			window_style       window;
			slider_style       slider;
			text_control_style text_control;			
		};

		style make_default_style(void);
	}
}