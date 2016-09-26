#include <xng/gui/style.hpp>
#include <xng/core.hpp>

using namespace xng::gui;
using namespace xng::math;

style xng::gui::make_default_style(void)
{
	style s = {};

	s.window_background_color = float4(.224f, .226f, .188f, 1.f);

	s.caption_background_color = float4(.45f, .847f, .0549f, 1.f);
	s.caption_height           = 26;

	s.caption_font              = "./fonts/OpenSans-Regular-64.xml";
	s.caption_text_origin       = uint2(8, 20);
	s.caption_text_color        = float4(1);
	s.caption_text_border_color = float4(0, 0, 0, 1);
	s.caption_text_border_size  = 0;
	s.caption_text_thinness     = .5f;
	s.caption_text_scale        = .25f;

	s.slider_size        = uint2(14);
	s.slider_color       = float4(.45f, .847f, .0549f, 1.f);
	s.slider_hover_color = float4(.65f, .9f, .27f, 1.f);
	s.slider_bar_filled  = float4(.33f, .63f, .12f, 1.f);
	s.slider_bar_empty   = float4(.05f, .065f, .051f, 1.f);
	s.slider_bar_height  = 12;

	s.text_control_font                      = "./fonts/OpenSans-Regular-64.xml";
	s.text_control_padding                   = 2;
	s.text_control_background_color          = float4(0);
	s.text_control_editable_background_color = float4(1);
	s.text_control_text_color                = float4(0, 0, 0, 1);
	s.text_control_text_border_size          = 0;
	s.text_control_text_thinness             = .5f;
	s.text_control_text_scale                = .2f;

	return s;
}