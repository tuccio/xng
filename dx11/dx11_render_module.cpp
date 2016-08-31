#include <xng/dx11/dx11_render_module.hpp>
#include <xng/dx11/gpu_mesh_manager.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;

const char *          dx11_render_module::module_name        = "xngdx11";
const char *          dx11_render_module::module_description = "XNG DirectX 11 Render Module";
const xng_module_type dx11_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool dx11_render_module::init(native_window * window)
{
	bool debug = false;
#ifdef XNG_DX11_DEBUG
	debug = true;
#endif
	m_context  = std::make_unique<dx11_api_context>();
	m_renderer = std::make_unique<forward_renderer>();

	if (m_context->init(window->get_native_handle(), XNG_API_DIRECTX_11, debug) && m_renderer->init(m_context.get()))
	{
		m_window         = window;
		m_windowObserver = realtime_window_observer(&m_configuration, m_context.get());
		window->add_observer(&m_windowObserver);

		auto clientSize = window->get_client_size();
		m_context->on_resize(clientSize.x, clientSize.y);

		resource_factory::get_singleton()->register_manager(new gpu_mesh_manager(m_context->get_device()));

		return true;
	}
	else
	{
		m_renderer.reset();
		m_context.reset();
		return false;
	}
}

void dx11_render_module::shutdown(void)
{
	m_window->remove_observer(&m_windowObserver);
	m_window = nullptr;

	delete resource_factory::get_singleton()->unregister_manager("dx11mesh");

	m_renderer->shutdown();

	m_renderer.reset();
	m_context.reset();
}

bool dx11_render_module::is_initialized(void) const
{
	return m_context && m_renderer;
}

void dx11_render_module::render(scene * scene, const camera * camera)
{
	render_variables rvars;
	render_variables_updates updates;

	configuration().get_render_variables(&rvars, &updates);

	m_context->frame_start();
	m_renderer->update_render_variables(rvars, updates);
	m_renderer->render(scene, camera);
	m_context->frame_complete();
}

api_context * dx11_render_module::get_api_context(void)
{
	return m_context.get();
}