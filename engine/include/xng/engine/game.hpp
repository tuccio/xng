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

			XNG_INLINE scene_module * get_scene_module(void) const
			{
				return m_scene;
			}

			XNG_INLINE void set_scene_module(scene_module * scene)
			{
				m_scene = scene;
			}

			XNG_INLINE render_module  * get_render_module(void) const
			{
				return m_render;
			}

			XNG_INLINE void set_render_module(render_module * render)
			{
				m_render = render;
			}

			XNG_INLINE runtime_module * get_runtime_module(void) const
			{
				return m_runtime;
			}

			XNG_INLINE void set_runtime_module(runtime_module * runtime)
			{
				m_runtime = runtime;
			}

			XNG_INLINE os::native_window * get_window(void) const
			{
				return m_window.get();
			}

		private:

			std::unique_ptr<os::native_window> m_window;
			std::unique_ptr<os::main_loop>     m_mainLoop;
			os::high_resolution_timer<float>   m_timer;

			scene_module   * m_scene;
			render_module  * m_render;
			runtime_module * m_runtime;

			std::atomic<bool> m_running;

			graphics::scene         * m_renderScene;
			std::mutex                m_renderMutex;
			std::condition_variable   m_renderCV;
			std::thread               m_renderingThread;

			void rendering_loop(void);

		};
	}
}