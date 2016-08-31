#include <xng/dx11/dx11_render_module.hpp>
#include <xng/dx11/gpu_mesh_manager.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;

const char *          dx11_render_module::module_name        = "xngdx11";
const char *          dx11_render_module::module_description = "XNG DirectX 11 Render Module";
const xng_module_type dx11_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool dx11_render_module::init(native_window_handle window, xng_api_version version)
{
	bool debug = false;
#ifdef XNG_DX11_DEBUG
	debug = true;
#endif
	m_context  = std::make_unique<dx11_api_context>();
	m_renderer = std::make_unique<forward_renderer>();

	if (!m_context->init(window, version, debug) || !m_renderer->init(m_context.get()))
	{
		m_renderer.reset();
		m_context.reset();
		return false;
	}

	resource_factory::get_singleton()->register_manager(new gpu_mesh_manager(m_context->get_device()));

	return true;
}

void dx11_render_module::shutdown(void)
{
	delete resource_factory::get_singleton()->unregister_manager("dx11mesh");

	m_renderer->shutdown();

	m_renderer.reset();
	m_context.reset();
}

bool dx11_render_module::is_initialized(void) const
{
	return m_context && m_renderer;
}

void dx11_render_module::render(scene * scene, const camera * camera, render_resource * resource)
{
	m_context->frame_start();
	m_renderer->render(scene, camera, resource);
	m_context->frame_complete();
}

api_context * dx11_render_module::get_api_context(void)
{
	return m_context.get();
}