#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/gl_render_module.hpp>
#include <xng/gl/wgl_api_context.hpp>
#include <xng/gl/gpu_mesh_manager.hpp>

using namespace xng::gl;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;

const char *          gl_render_module::module_name        = "xnggl45";
const char *          gl_render_module::module_description = "XNG OpenGL 4.5 Render Module";
const xng_module_type gl_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool gl_render_module::init(native_window_handle window, xng_api_version version)
{
	bool debug = false;
#ifdef XNG_DX11_DEBUG
	debug = true;
#endif
	m_context  = std::unique_ptr<gl_api_context>(new wgl_api_context);
	m_renderer = std::make_unique<forward_renderer>();

	if (!m_context->init(window, XNG_API_GL_4_5, debug) || !m_renderer->init(m_context.get()))
	{
		m_renderer.reset();
		m_context.reset();
		return false;
	}

	resource_factory::get_singleton()->register_manager(new gpu_mesh_manager);

	return true;
}

void gl_render_module::shutdown(void)
{
	delete resource_factory::get_singleton()->unregister_manager("glmesh");

	m_renderer->shutdown();

	m_renderer.reset();
	m_context.reset();
}

bool gl_render_module::is_initialized(void) const
{
	return m_context && m_renderer;
}

void gl_render_module::render(scene * scene, const camera * camera, render_resource * resource)
{
	m_context->frame_start();
	m_renderer->render(scene, camera, resource);
	m_context->frame_complete();
}

api_context * gl_render_module::get_api_context(void)
{
	return m_context.get();
}