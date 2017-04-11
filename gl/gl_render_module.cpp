#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/gl_render_module.hpp>
#include <xng/gl/wgl_api_context.hpp>
#include <xng/gl/gpu_mesh_manager.hpp>

using namespace xng::gl;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;
using namespace xng::gui;

const char *          gl_render_module::module_name        = "xnggl45";
const char *          gl_render_module::module_description = "XNG OpenGL 4.5 Render Module";
const xng_module_type gl_render_module::module_type        = XNG_MODULE_TYPE_RENDER;

bool gl_render_module::init(native_window * window_body)
{
    bool debug = false;
#ifdef XNG_DX11_DEBUG
    debug = true;
#endif
    m_context  = std::unique_ptr<gl_api_context>(xng_new wgl_api_context);
    m_renderer = std::make_unique<forward_renderer>();

    if (m_context->init(window_body->get_native_handle(), XNG_API_GL_4_5, debug) && m_renderer->init(m_context.get()))
    {
        m_window = window_body;
        m_windowObserver = realtime_window_observer(&m_configuration, m_context.get());
        window_body->add_observer(&m_windowObserver);

        m_windowObserver.on_resize(m_window, m_window->get_window_size(), m_window->get_client_size());

        resource_factory::get_singleton()->register_manager(xng_new gpu_mesh_manager(m_context.get()));

        m_context->dispose();
        return true;
    }
    else
    {
        m_renderer.reset();
        m_context.reset();
        return false;
    }
}

void gl_render_module::shutdown(void)
{
    m_window->remove_observer(&m_windowObserver);
    m_window = nullptr;

    m_context->use();

    xng_delete resource_factory::get_singleton()->unregister_manager("glmesh");
    m_renderer->shutdown();

    m_context->dispose();

    m_renderer.reset();
    m_context.reset();
}

bool gl_render_module::is_initialized(void) const
{
    return m_context && m_renderer;
}

void gl_render_module::render(const extracted_scene & scene, const gui_command_list & guiCommandList)
{
    render_variables rvars;
    render_variables_updates updates;

    m_windowObserver.update(&rvars, &updates);

    m_context->use();

    m_context->frame_start();

    m_renderer->update_render_variables(rvars, updates);

    m_renderer->render(scene);

    m_context->frame_complete();

    m_context->dispose();
}

api_context * gl_render_module::get_api_context(void)
{
    return m_context.get();
}