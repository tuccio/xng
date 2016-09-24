#include <xng/gui/image_control.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::graphics;
using namespace xng::res;

image_control::image_control(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_IMAGE_CONTROL, position, size) {}

void image_control::extract(gui_command_list_inserter & inserter, const style & style) const
{
	if (m_image)
	{
		*inserter++ = make_textured_rectangle_command(get_rectangle(), float2(0), float2(1), m_image);
	}
	else
	{
		*inserter++ = make_filled_rectangle_command(get_rectangle(), float4(0, 0, 0, 1));
	}
}

image_ptr image_control::get_image(void) const
{
	return m_image;
}

void image_control::set_image(image_ptr image)
{
	m_image = image;
}

void image_control::fit(void)
{
	if (m_image && m_image->load())
	{
		set_size(m_image->get_size());
	}
}