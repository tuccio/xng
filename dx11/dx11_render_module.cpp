#include <xng/dx11/dx11_render_module.hpp>
#include <xng/dx11/gpu_mesh.hpp>
#include <xng/dx11/texture.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;
using namespace xng::gui;

const char *          dx11_render_module::module_name        = "xngdx11";
const char *          dx11_render_module::module_description = "XNG DirectX 11 Render Module";
const xng_module_type dx11_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool dx11_render_module::init(native_window * window_body)
{
	bool debug = false;
#ifdef XNG_DX11_DEBUG
	debug = true;
#endif

	m_context     = std::make_unique<dx11_api_context>();
	m_renderer    = std::make_unique<forward_renderer>();
	m_guiRenderer = std::make_unique<dx11_gui_renderer>();

	if (m_context->init(window_body->get_native_handle(), XNG_API_DIRECTX_11, debug) &&
		m_guiRenderer->init(m_context.get()) &&
		m_renderer->init(m_context.get()))
	{
		m_window         = window_body;
		m_windowObserver = realtime_window_observer(&m_configuration, m_context.get());
		window_body->add_observer(&m_windowObserver);

		m_windowObserver.on_resize(m_window, m_window->get_window_size(), m_window->get_client_size());

		auto clientSize = window_body->get_client_size();
		m_context->on_resize(clientSize.x, clientSize.y);

		resource_factory::get_singleton()->register_manager(xng_new gpu_mesh_manager());
		resource_factory::get_singleton()->register_manager(xng_new texture_manager());
		
		if (m_samplers.init(m_context->get_device()))
		{
			ID3D11DeviceContext * immediateContext = m_context->get_immediate_context();
			m_samplers.bind_vertex_shader(immediateContext);
			m_samplers.bind_pixel_shader(immediateContext);
			m_samplers.bind_compute_shader(immediateContext);
		}

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

	m_samplers.shutdown();

	m_renderer->shutdown();
	m_guiRenderer->shutdown();

	m_renderer.reset();
	m_guiRenderer.reset();

	resource_manager * gpuMeshManager = resource_factory::get_singleton()->unregister_manager(gpu_mesh::resource_type);
	resource_manager * textureManager = resource_factory::get_singleton()->unregister_manager(texture::resource_type);

	gpuMeshManager->clear();
	textureManager->clear();

	xng_delete gpuMeshManager;
	xng_delete textureManager;

	m_context.reset();
}

bool dx11_render_module::is_initialized(void) const
{
	return m_context && m_renderer;
}

void dx11_render_module::render(scene * scene, gui_manager * gui)
{
	render_variables rvars;
	render_variables_updates updates;

	m_windowObserver.update(&rvars, &updates);

	m_context->frame_start();

	m_renderer->update_render_variables(rvars, updates);

	scene_graph_camera * cameraNode = scene ? scene->get_active_camera() : nullptr;
	camera             * camera     = cameraNode ? cameraNode->get_camera() : nullptr;

	if (camera)
	{
		float ratio = rvars.render_resolution.x / (float)rvars.render_resolution.y;
		camera->set_aspect_ratio(ratio);
	}

	m_renderer->render(m_context->get_immediate_context(), scene, camera);

	if (gui)
	{
		gui->set_renderer(m_guiRenderer.get());
		gui->render();
	}
	
	m_context->frame_complete();
}

api_context * dx11_render_module::get_api_context(void)
{
	return m_context.get();
}