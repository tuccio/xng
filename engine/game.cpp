#include <xng/engine/game.hpp>

#include <functional>
#include <thread>
#include <mutex>

using namespace xng::engine;
using namespace xng::graphics;
using namespace xng::os;

game::game(void) :
	m_scene(nullptr),
	m_render(nullptr),
	m_runtime(nullptr),
	m_running(false),
	m_rendering(false),
	m_renderScene(nullptr)
{
}

bool game::init(void)
{
	assert(!m_running && !m_rendering && "Init called while the game is still running.");

	m_window   = std::make_unique<native_window>();
	m_mainLoop = std::make_unique<main_loop>();

	if (m_window->create())
	{
#define XNG_GAME_INIT_MODULE(Module, ...) if (Module && !Module->is_initialized()) { bool success = Module->init(__VA_ARGS__); if (!success) { XNG_LOG("Module initialization failed:", #Module); return false; } }

		XNG_GAME_INIT_MODULE(m_runtime);
		XNG_GAME_INIT_MODULE(m_scene);
		XNG_GAME_INIT_MODULE(m_render, m_window.get());

#undef XNG_GAME_INIT_MODULE

		return true;
	}

	return false;
}

struct game_window_observer :
	public native_window_observer
{
	game_window_observer(main_loop * loop) : m_loop(loop) {}

	void on_destroy(native_window * window)
	{
		m_loop->quit();
	}

private:

	main_loop * m_loop;
};

void game::shutdown(void)
{
	assert(!m_running && !m_rendering && "Shutdown called while the game is still running.");

#define XNG_GAME_SHUTDOWN_MODULE(Module, ...) if (Module && Module->is_initialized()) { Module->shutdown(); }

	XNG_GAME_SHUTDOWN_MODULE(m_runtime);
	XNG_GAME_SHUTDOWN_MODULE(m_scene);
	XNG_GAME_SHUTDOWN_MODULE(m_render);

#undef XNG_GAME_SHUTDOWN_MODULE

	m_quitOnClose.reset();
	m_window.reset();
	m_mainLoop.reset();
}

void game::run(void)
{
	m_mainLoop->set_idle_callback(
		[this]()
	{
		// Actual game loop

		float dt = m_timer.get_elapsed_seconds();

		if (m_runtime)
		{
			m_runtime->update(dt);
		}

		if (m_scene)
		{
			scene * currentScene = m_scene->get_active_scene();

			if (currentScene)
			{
				currentScene->update();

				if (m_render)
				{
					std::lock_guard<std::mutex> renderLock(m_renderMutex);
					m_renderScene = currentScene->clone();
					m_renderCV.notify_all();
				}
			}
		}
	});

	start_rendering();

	m_running = true;

	m_timer.get_elapsed_seconds();
	m_mainLoop->run();

	m_running = false;

	stop_rendering();
}

bool game::is_running(void) const
{
	return m_running;
}

void game::start_rendering(void)
{
	if (!m_rendering)
	{
		// Make sure no previous rendering thread is running

		if (m_renderingThread.joinable())
		{
			m_renderingThread.join();
		}

		// Start the rendering thread

		m_rendering = true;
		m_renderingThread = std::thread(std::bind(&game::rendering_loop, this));
	}
}

void game::stop_rendering(void)
{
	// Signal the rendering thread to stop and wait for it to terminate

	if (m_rendering)
	{
		m_rendering = false;

		if (m_renderMutex.try_lock())
		{
			m_renderCV.notify_all();
			m_renderMutex.unlock();
		}

		m_renderingThread.join();
	}
}

bool game::is_rendering(void) const
{
	return m_rendering;
}

void game::rendering_loop(void)
{
	do
	{
		std::unique_lock<std::mutex> renderLock(m_renderMutex);

		while (!m_renderScene && m_rendering)
		{
			m_renderCV.wait(renderLock);
		}

		if (m_renderScene)
		{
			m_render->render(m_renderScene);
			delete m_renderScene;
			m_renderScene = nullptr;
		}
	} while (m_rendering);
}

void game::set_quit_on_close(bool quitOnClose)
{
	if (m_window && m_window->exists())
	{
		game_window_observer windowObserver(m_mainLoop.get());
		m_window->add_observer(&windowObserver);
	}
}

bool game::get_quit_on_close(void) const
{
	return (bool)m_quitOnClose;
}

void game::quit(void)
{
	m_mainLoop->quit();
}

scene_module * game::get_scene_module(void) const
{
	return m_scene;
}

void game::set_scene_module(scene_module * scene)
{
	m_scene = scene;
}

render_module * game::get_render_module(void) const
{
	return m_render;
}

void game::set_render_module(render_module * render)
{
	m_render = render;
}

runtime_module * game::get_runtime_module(void) const
{
	return m_runtime;
}

void game::set_runtime_module(runtime_module * runtime)
{
	m_runtime = runtime;
}

native_window * game::get_window(void) const
{
	return m_window.get();
}