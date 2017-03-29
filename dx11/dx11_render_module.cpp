 #include <xng/dx11/dx11_render_module.hpp>
#include <xng/dx11/gpu_mesh.hpp>
#include <xng/dx11/texture.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;
using namespace xng::gui;
using namespace xng::math;

const char *          dx11_render_module::module_name        = "xngdx11";
const char *          dx11_render_module::module_description = "XNG DirectX 11 Render Module";
const xng_module_type dx11_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

static std::atomic<bool> g_reloadShaders;

static struct shader_refresh_observer :
	native_window_observer
{
	void on_keyboard_key_down(native_window * wnd, xng_keyboard_key key) override
	{
		if (key == XNG_KEYBOARD_F5)
		{
			g_reloadShaders = true;
		}
	}
} g_shaderRefresher;

bool dx11_render_module::init(native_window * wnd)
{
	bool debug = false;
#ifdef XNG_DX11_DEBUG
	debug = true;
#endif

	m_context        = std::make_unique<dx11_api_context>();
	m_renderer       = std::make_unique<forward_renderer>();
	m_guiRenderer    = std::make_unique<dx11_gui_renderer>();
	m_visualDebugger = std::make_unique<visual_debugger>();

	render_variables rvars;
	m_configuration.get_render_variables(&rvars, nullptr);

	if (m_context->init(wnd->get_native_handle(), XNG_API_DIRECTX_11, debug) &&
		m_guiRenderer->init(m_context.get()) &&
		m_renderer->init(m_context.get(), rvars) &&
		m_visualDebugger->init(m_context.get(), rvars))
	{
		m_window         = wnd;
		m_windowObserver = realtime_window_observer(&m_configuration, m_context.get());

		wnd->add_observer(&m_windowObserver);
		wnd->add_observer(&g_shaderRefresher);

		m_windowObserver.on_resize(m_window, m_window->get_window_size(), m_window->get_client_size());

		auto clientSize = wnd->get_client_size();
		m_context->on_resize(clientSize.x, clientSize.y);

		resource_factory::get_singleton()->register_manager(xng_new gpu_mesh_manager());
		resource_factory::get_singleton()->register_manager(xng_new texture_manager());
		
		return m_samplers.init(m_context->get_device());
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
	m_window->remove_observer(&g_shaderRefresher);
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

	std::for_each(updates.begin(), updates.end(),
		[&](xng_render_variable rvar)
	{
		switch (rvar)
		{
		case XNG_RV_RENDER_RESOLUTION:
			m_renderResourceManager.clear();
			break;

		case XNG_RV_DEBUG_CAMERA_FRUSTUM:
			if (rvars.debug_camera_frustum)
			{
				m_visualDebugger->add_persistent("debug_camera_frustum", extractedScene.get_active_camera()->get_frustum(), float4(0, 1, 0, 1));
			}
			else
			{
				m_visualDebugger->remove_persistent("debug_camera_frustum");
			}
			break;

		default:
			break;
		}
	});

	m_context->frame_start();

	bool expected = true;

	if (g_reloadShaders.compare_exchange_weak(expected, false))
	{
		m_renderer->reload_shaders();
		m_guiRenderer->reload_shaders();
	}

	// Grab the resources needed

	ID3D11RenderTargetView * backBuffer = m_context->get_back_buffer_rtv();

	render_resource_handle_ptr depthBufferHandle = m_renderResourceManager.get_texture_2d(
		m_context->get_device(),
		CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_D24_UNORM_S8_UINT, rvars.render_resolution.x, rvars.render_resolution.y, 1, 0, D3D11_BIND_DEPTH_STENCIL));

	const render_resource * depthBuffer = depthBufferHandle->get_render_resource();

	// Setup common samplers

	ID3D11DeviceContext * immediateContext = m_context->get_immediate_context();

	m_samplers.bind_vertex_shader(immediateContext);
	m_samplers.bind_pixel_shader(immediateContext);
	m_samplers.bind_compute_shader(immediateContext);

	// Render

	m_renderer->update_render_variables(rvars, updates);

	if (rvars.forward_depth_prepass)
	{
		m_renderer->depth_prepass(immediateContext,
		                          depthBuffer->get_depth_stencil_view(),
		                          extractedScene);

		m_renderer->render(immediateContext,
		                   backBuffer,
		                   depthBuffer->get_depth_stencil_view(),
		                   false,
		                   extractedScene,
		                   rvars.debug_normals);
	}
	else
	{
		m_renderer->render(immediateContext,
		                   backBuffer,
		                   depthBuffer->get_depth_stencil_view(),
		                   true,
		                   extractedScene,
		                   rvars.debug_normals);
	}
	
	m_guiRenderer->render(immediateContext, backBuffer, rvars.render_resolution, guiCommandList);
	m_visualDebugger->render(immediateContext, backBuffer, depthBuffer->get_depth_stencil_view(), extractedScene.get_active_camera());
	
	m_context->frame_complete();
}

api_context * dx11_render_module::get_api_context(void)
{
	return m_context.get();
}