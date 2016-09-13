#include <xng/gui/rectangle.hpp>

using namespace xng::gui;
using namespace xng::math;

bool xng::gui::rectangle_contains(const rectangle & r, const int2 & x)
{
	return x.x >= r.left && x.y >= r.top &&
		x.y <= r.bottom && x.x <= r.right;
}


int32_t xng::gui::rectangle_width(const rectangle & r)
{
	return r.right - r.left;
}

int32_t xng::gui::rectangle_height(const rectangle & r)
{
	return r.bottom - r.top;
}

int2 xng::gui::rectangle_size(const rectangle & r)
{
	return r.bottomRight - r.topLeft;
}