#pragma once

#include <xng/gui/widget_enum.hpp>
#include <xng/math.hpp>

#include <memory>
#include <unordered_map>

#define XNG_STYLE_STRING_BUFFER_SIZE 256

namespace xng
{
	namespace gui
	{
		struct style
		{
			math::float4 window_background_color;

			math::float4 caption_background_color;
			uint32_t     caption_height;

			char         caption_font[XNG_STYLE_STRING_BUFFER_SIZE];

			math::uint2  caption_text_origin;
			math::float4 caption_text_color;
			math::float4 caption_text_border_color;
			uint32_t     caption_text_border_size;
			float        caption_text_smoothness;
			float        caption_text_scale;

			math::uint2  slider_size;
			math::float4 slider_color;
			math::float4 slider_bar_filled;
			math::float4 slider_bar_empty;
			uint32_t     slider_bar_height;
		};

		style make_default_style(void);
	}
}