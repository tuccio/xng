#pragma once

#include <xng/res.hpp>
#include <xng/gui/rectangle.hpp>
#include <xng/graphics.hpp>

namespace xng
{
	namespace gui
	{
		class gui_renderer
		{

		public:

			virtual void render_begin(const math::uint2 & size) = 0;
			virtual void render_end(void) = 0;

			virtual void render_filled_rectangle(const rectangle & rect, const math::float4 & color) = 0;

			virtual void render_textured_rectangle(
				const rectangle & rect,
				const math::float2 & uv0,
				const math::float2 & uv1,
				graphics::image_ptr texture) = 0;

			virtual void render_text(
				graphics::font_ptr fnt,
				const wchar_t * text,
				const math::float4 & color,
				const math::float4 & borderColor, uint32_t borderSize,
				float smoothness,
				const math::uint2 & position, const math::float2 & scale = math::float2(1)) = 0;

		};
	}
}