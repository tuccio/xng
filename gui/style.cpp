#include <xng/gui/style.hpp>
#include <xng/core.hpp>

using namespace xng::gui;
using namespace xng::math;

style xng::gui::make_default_style(void)
{
	style s = {};

	s.window_background_color  = float4(.224f, .226f, .188f, 1.f);

	s.caption_background_color = float4(.45f, .847f, .0549f, 1.f);
	s.caption_height           = 26;

	return s;
}