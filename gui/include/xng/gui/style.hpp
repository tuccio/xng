#pragma once

#include <xng/gui/widget_enum.hpp>
#include <xng/math.hpp>

#include <memory>
#include <unordered_map>

namespace xng
{
	namespace gui
	{
		struct style
		{
			math::float4 window_background_color;
			math::float4 caption_background_color;
			uint32_t     caption_height;
		};

		style make_default_style(void);
	}
}