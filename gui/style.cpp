#include <xng/gui/style.hpp>
#include <xng/core.hpp>

using namespace xng::gui;
using namespace xng::math;

style xng::gui::make_default_style(void)
{
	style s                                  = {};

	s.window.background_color                = float4(.224f, .226f, .188f, 1.f);

	s.window.caption.background_color        = float4(.45f, .847f, .0549f, 1.f);
	s.window.caption.height                  = 26;

	s.window.caption.text.font               = "./fonts/OpenSans-Regular-64.xml";
	s.window.caption.text.origin             = uint2(8, 20);
	s.window.caption.text.color              = float4(1);
	s.window.caption.text.border_color       = float4(0, 0, 0, 1);
	s.window.caption.text.border_size        = 0;
	s.window.caption.text.thinness           = .5f;
	s.window.caption.text.scale              = .25f;

	s.slider.size                            = uint2(14);
	s.slider.color                           = float4(.45f, .847f, .0549f, 1.f);
	s.slider.hover_color                     = float4(.65f, .9f, .27f, 1.f);
	s.slider.bar_filled_color                = float4(.33f, .63f, .12f, 1.f);
	s.slider.bar_empty_color                 = float4(.05f, .065f, .051f, 1.f);
	s.slider.bar_height                      = 12;

	s.text_control.text.font                 = "./fonts/OpenSans-Regular-64.xml";
	s.text_control.padding                   = 2;
	s.text_control.background_color          = float4(0);
	s.text_control.editable_background_color = float4(1);
	s.text_control.text.color                = float4(.015f, .042f, .037f, 1.f);
	s.text_control.text.border_size          = 0;
	s.text_control.text.thinness             = .5f;
	s.text_control.text.scale                = .2f;

	return s;
}