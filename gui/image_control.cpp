#include <xng/gui/image_control.hpp>
#include <xng/gui/gui_manager.hpp>

using namespace xng::gui;
using namespace xng::math;
using namespace xng::graphics;
using namespace xng::res;

image_control::image_control(gui_manager * manager, widget * parent, const int2 & position, const int2 & size) :
	widget(manager, parent, XNG_GUI_IMAGE_CONTROL, position, size) {}

void image_control::render(gui_renderer * renderer, const style & style) const
{
	if (m_image)
	{
		renderer->render_textured_rectangle(get_rectangle(), float2(0), float2(1), m_image);
	}
	else
	{
		renderer->render_filled_rectangle(get_rectangle(), float4(0, 0, 0, 1));
	}
}

image_control * image_control::clone(gui_manager * manager, widget * parent) const
{
	image_control * newWidget = xng_new image_control(*this);

	newWidget->set_parent(parent);
	newWidget->set_gui_manager(manager);

	clone_children(manager, newWidget);

	return newWidget;
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