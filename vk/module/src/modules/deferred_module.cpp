#include <xng/vk/modules/deferred_module.hpp>

using namespace xng::vk;
using namespace xng;

const char *          deferred_module::module_name        = "xngvk-deferred";
const char *          deferred_module::module_description = "XNG Vulkan Deferred Rendering Module";
const xng_module_type deferred_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool deferred_module::init(os::native_window * window_body)
{
	m_context.reset(new api_context);
	return true;
}

void deferred_module::shutdown(void)
{

}

bool deferred_module::is_initialized(void) const
{
	return true;
}

void deferred_module::render(const graphics::extracted_scene & extractedScene, const gui::gui_command_list & guiCommandList)
{

}

graphics::api_context * deferred_module::get_api_context(void)
{
	return m_context.get();
}