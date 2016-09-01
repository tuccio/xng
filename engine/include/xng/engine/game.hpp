#pragma once

#include <xng/engine/scene_module.hpp>
#include <xng/engine/render_module.hpp>
#include <xng/engine/runtime_module.hpp>

#include <xng/os.hpp>

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

			void run(void);
			bool is_running(void) const;
			void quit(void);

			scene_module * get_scene_module(void) const;
			void set_scene_module(scene_module * scene);

			render_module * get_render_module(void) const;
			void set_render_module(render_module * render);

			runtime_module * get_runtime_module(void) const;
			void set_runtime_module(runtime_module * runtime);

			os::native_window * get_window(void) const;

			void set_quit_on_close(bool quitOnClose);
			bool get_quit_on_close(void) const;

			void start_rendering(void);
			void stop_rendering(void);
			bool is_rendering(void) const;

		private:

			std::unique_ptr<os::native_window> m_window;
			std::unique_ptr<os::main_loop>     m_mainLoop;
			os::high_resolution_timer<float>   m_timer;

			std::unique_ptr<os::native_window_observer> m_quitOnClose;

			scene_module   * m_scene;
			render_module  * m_render;
			runtime_module * m_runtime;

			std::atomic<bool> m_running;
			std::atomic<bool> m_rendering;

			graphics::scene         * m_renderScene;
			std::mutex                m_renderMutex;
			std::condition_variable   m_renderCV;
			std::thread               m_renderingThread;

			void rendering_loop(void);

		};
	}
}