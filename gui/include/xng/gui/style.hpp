#pragma once

#include <xng/gui/widget_enum.hpp>
#include <xng/math.hpp>

#include <string>

namespace xng
{
	namespace gui
	{
		struct style
		{
			math::float4 window_background_color;

			math::float4 caption_background_color;
			uint32_t     caption_height;

			std::string  caption_font;
			math::uint2  caption_text_origin;
			math::float4 caption_text_color;
			math::float4 caption_text_border_color;
			uint32_t     caption_text_border_size;
			float        caption_text_thinness;
			float        caption_text_scale;

			math::uint2  slider_size;
			math::float4 slider_color;
			math::float4 slider_hover_color;
			math::float4 slider_bar_filled;
			math::float4 slider_bar_empty;
			uint32_t     slider_bar_height;

			std::string  text_control_font;
			math::float4 text_control_background_color;
			math::float4 text_control_editable_background_color;
			math::float4 text_control_text_color;
			math::float4 text_control_text_border_color;
			uint32_t     text_control_text_border_size;
			float        text_control_text_thinness;
			float        text_control_text_scale;
			uint32_t     text_control_padding;
		};

		style make_default_style(void);
	}
}