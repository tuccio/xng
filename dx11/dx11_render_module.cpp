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

void dx11_render_module::render(const extracted_scene & extractedScene, const gui_command_list & guiCommandList)
{
	render_variables rvars;
	render_variables_updates updates;

	// Update render variables

	m_windowObserver.update(&rvars, &updates);

	if (updates.find(XNG_RV_RENDER_RESOLUTION) != updates.end())
	{
		m_renderResourceManager.clear();
	}

	m_context->frame_start();

	// Grab the resources needed

	ID3D11RenderTargetView * backBuffer = m_context->get_back_buffer_rtv();

	render_resource_handle_ptr depthBufferHandle = m_renderResourceManager.get_texture_2d(
		m_context->get_device(),
		CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_D24_UNORM_S8_UINT, rvars.render_resolution.x, rvars.render_resolution.y, 1, 0, D3D11_BIND_DEPTH_STENCIL));

	const render_resource * depthBuffer = depthBufferHandle->get_render_resource();

	// Render

	m_renderer->update_render_variables(rvars, updates);

	m_renderer->render(m_context->get_immediate_context(), backBuffer, depthBuffer->get_depth_stencil_view(), extractedScene);
	m_guiRenderer->render(m_context->get_immediate_context(), backBuffer, rvars.render_resolution, guiCommandList);
	
	m_context->frame_complete();
}

api_context * dx11_render_module::get_api_context(void)
{
	return m_context.get();
}