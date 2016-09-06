#include <xng/engine/game.hpp>

#include <functional>
#include <thread>
#include <mutex>

using namespace xng::engine;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::input;

/* Observers used */

struct game_window_observer :
	public native_window_observer
{
	game_window_observer(main_loop * loop) :
		m_loop(loop) {}

	void on_destroy(native_window * window)
	{
		m_loop->quit();
	}

private:

	main_loop * m_loop;
};

struct game_input_observer :
	public native_window_observer
{
	game_input_observer(input_handler * inputHandler) :
		m_inputHandler(inputHandler) {}

	void on_keyboard_key_down(native_window * window, xng_keyboard_key key)
	{
		m_inputHandler->on_keyboard_key_down(key, timestamp());
	}

	void on_keyboard_key_up(native_window * window, xng_keyboard_key key)
	{
		m_inputHandler->on_keyboard_key_up(key, timestamp());
	}

	void on_focus(native_window * window)
	{
		m_inputHandler->on_focus_change(timestamp());
	}

	void on_unfocus(native_window * window)
	{
		m_inputHandler->on_focus_change(timestamp());
	}

private:

	input_handler * m_inputHandler;
};

struct game_keyboard_observer :
	public keyboard_observer
{
	bool on_keyboard_key_down(xng_keyboard_key key)
	{
		XNG_LOG("Pressed key", XNG_LOG_STREAM() << key);
		return true;
	}

	bool on_keyboard_key_up(xng_keyboard_key key, uint32_t millis)
	{
		XNG_LOG("Released key", XNG_LOG_STREAM() << key << " after " << millis << "ms.");
		return true;
	}
};

/* Game class implementation */

game::game(void) :
	m_scene(nullptr),
	m_render(nullptr),
	m_runtime(nullptr),
	m_running(false),
	m_rendering(false),
	m_renderScene(nullptr),
	m_ticksPerSecond(30)
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

void game::shutdown(void)
{
	assert(!m_running && !m_rendering && "Shutdown called while the game is still running.");

#define XNG_GAME_SHUTDOWN_MODULE(Module, ...) if (Module && Module->is_initialized()) { Module->shutdown(); xng_delete Module; Module = nullptr; }

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
	start_rendering();

	m_running = true;

	m_gameLoopThread = std::thread(std::bind(&game::game_loop, this));
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
	while (m_rendering)
	{
		std::unique_lock<std::mutex> renderLock(m_renderMutex);

		while (!m_renderScene && m_rendering)
		{
			m_renderCV.wait(renderLock);
		}

		if (m_renderScene)
		{
			m_render->render(m_renderScene);
			xng_delete m_renderScene;
			m_renderScene = nullptr;
			m_renderCV.notify_all();
		}
	}
}

void game::game_loop(void)
{
	const high_resolution_timestamp GameStart = timestamp();

	const uint32_t TicksPerSecond = m_ticksPerSecond;
	const uint32_t MaxFrameSkip   = 10;

	const uint32_t SkipTicks      = 1000 / TicksPerSecond;
	const float    TickSeconds    = 1.f / TicksPerSecond;

	uint32_t nextTick = SkipTicks;

	scene * currentScene = nullptr;

	game_keyboard_observer keyboardObserver;

	m_inputHandler.keyboard().add_observer(&keyboardObserver);

	game_input_observer inputObserver(&m_inputHandler);
	m_window->add_observer(&inputObserver);

	while (m_running)
	{
		for (uint32_t loops = 0;
			to_milliseconds<uint32_t>(timestamp() - GameStart) > nextTick && loops < MaxFrameSkip;
			++loops)
		{
			m_inputHandler.dispatch(GameStart + nextTick);

			if (m_runtime)
			{
				m_runtime->update(TickSeconds);
			}

			if (m_scene)
			{
				currentScene = m_scene->get_active_scene();

				if (currentScene)
				{
					currentScene->update();
				}
			}

			nextTick += SkipTicks;
		}

		// Feed the rendering thread a new scene (TODO: interpolation)

		if (currentScene && m_rendering)
		{
			std::unique_lock<std::mutex> renderLock(m_renderMutex);

			while (m_renderScene && m_rendering)
			{
				m_renderCV.wait(renderLock);
			}

			if (!m_renderScene)
			{
				m_renderScene = currentScene->clone();
				m_renderCV.notify_all();
			}
		}
	}

	m_window->remove_observer(&inputObserver);
	m_inputHandler.clear();
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
	m_running = false;
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

void game::set_ticks_per_second(uint32_t ticks)
{
	m_ticksPerSecond = ticks;
}

uint32_t game::get_ticks_per_second(void) const
{
	return m_ticksPerSecond;
}