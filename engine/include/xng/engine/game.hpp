#pragma once

#include <xng/engine/scene_module.hpp>
#include <xng/engine/render_module.hpp>
#include <xng/engine/runtime_module.hpp>

#include <xng/gui.hpp>

#include <xng/input.hpp>

#include <xng/os.hpp>

#include <array>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace xng
{
    namespace engine
    {
        class game :
            public core::singleton<game>
        {

        public:

            game(void);

            bool init(void);
            void shutdown(void);

            void clear(void);

            void run(void);
            bool is_running(void) const;
            void quit(void);

            void pause(void);
            void unpause(void);

            scene_module * get_scene_module(void) const;
            void set_scene_module(scene_module * scene);

            render_module * get_render_module(void) const;
            void set_render_module(render_module * render);

            runtime_module * get_runtime_module(void) const;
            void set_runtime_module(runtime_module * runtime);

            os::native_window * get_window(void) const;

            gui::gui_manager * get_gui_manager(void) const;
            const input::input_handler * get_input_handler(void) const;

            void set_quit_on_close(bool quitOnClose);
            bool get_quit_on_close(void) const;

            void start_rendering(void);
            void stop_rendering(void);
            bool is_rendering(void) const;

            void set_ticks_per_second(uint32_t ticks);
            uint32_t get_ticks_per_second(void) const;

            float get_frames_per_second(void) const;
            float get_frame_time(void) const;

        private:

            std::unique_ptr<os::native_window> m_window;
            std::unique_ptr<gui::gui_manager>  m_guiManager;

            // Modules

            scene_module   * m_scene;
            render_module  * m_render;
            runtime_module * m_runtime;

            // Loop members

            uint32_t m_ticksPerSecond;

            std::atomic<bool> m_paused;
            std::atomic<bool> m_running;
            std::atomic<bool> m_rendering;

            std::unique_ptr<os::main_loop> m_mainLoop;
            bool                           m_renderReady;
            graphics::extracted_scene      m_extractedScene;
            gui::gui_command_list          m_guiCommandList;

            std::unique_ptr<os::native_window_observer> m_quitOnClose;

            std::mutex              m_renderMutex;
            std::condition_variable m_renderCV;
            std::thread             m_renderingThread;
            std::thread             m_gameLoopThread;

            std::unique_ptr<os::native_window_observer> m_inputObserver;

            // FPS

            std::array<os::high_resolution_timestamp, 15> m_fpsTimings;
            os::high_resolution_timestamp                 m_fpsLast;
            os::high_resolution_timestamp                 m_fpsSum;
            uint32_t                                      m_fpsIndex;

            // Input

            input::input_handler m_inputHandler;

            // Internal functions

            void rendering_loop(void);
            void game_loop(void);

            void init_fps_counter(void);
            void update_fps_counter(void);
        };
    }
}