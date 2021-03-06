#include <xng/engine.hpp>
#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/forward_renderer.hpp>

#include <memory>

namespace xng
{
    namespace gl
    {
        class gl_render_module :
            public engine::render_module
        {

        public:

            static const char *          module_name;
            static const char *          module_description;
            static const xng_module_type module_type;

            bool init(os::native_window * window_body) override;
            void shutdown(void) override;
            bool is_initialized(void) const override;

            void render(const graphics::extracted_scene & extractedScene, const gui::gui_command_list & guiCommandList) override;
            graphics::api_context * get_api_context(void) override;

        private:

            std::unique_ptr<gl_api_context>   m_context;
            std::unique_ptr<forward_renderer> m_renderer;

            os::native_window * m_window;

            graphics::realtime_window_observer m_windowObserver;

        };
    }
}