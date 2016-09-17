#include <xng/gui/style.hpp>
#include <xng/core.hpp>

using namespace xng::gui;
using namespace xng::math;

style xng::gui::make_default_style(void)
{
	style s = {};

	s.window_background_color   = float4(.224f, .226f, .188f, 1.f);

	s.caption_background_color  = float4(.45f, .847f, .0549f, 1.f);
	s.caption_height            = 26;

	strncpy(s.caption_font, "./fonts/OpenSans-16.xml", XNG_STYLE_STRING_BUFFER_SIZE);

	s.caption_text_origin       = uint2(8, 20);
	s.caption_text_color        = float4(1);
	s.caption_text_border_color = float4(0, 0, 0, 1);
	s.caption_text_border_size  = 2;
	s.caption_text_smoothness   = .5f;
	s.caption_text_scale        = 1.f;

	s.slider_size               = uint2(14);
	s.slider_color              = float4(.45f, .847f, .0549f, 1.f);
	s.slider_bar_filled         = float4(.33f, .63f, .12f, 1.f);
	s.slider_bar_empty          = float4(.05f, .065f, .051f, 1.f);
	s.slider_bar_height         = 12;

	return s;
}