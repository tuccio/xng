#include <xng/engine/game.hpp>

#include <functional>
#include <thread>
#include <mutex>

using namespace xng::engine;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::input;
using namespace xng::gui;

/* Observers used */

struct game_window_observer :
	public native_window_observer
{
	game_window_observer(main_loop * loop) :
		m_loop(loop) {}

	void on_destroy(native_window * window_body)
	{
		m_loop->quit();
	}

private:

	main_loop * m_loop;
};

struct game_window_input_observer :
	public native_window_observer
{
	game_window_input_observer(input_handler * inputHandler) :
		m_inputHandler(inputHandler) {}

	void on_keyboard_key_down(native_window * window_body, xng_keyboard_key key) override
	{
		m_inputHandler->on_keyboard_key_down(key, timestamp());
	}

	void on_keyboard_key_up(native_window * window_body, xng_keyboard_key key) override
	{
		m_inputHandler->on_keyboard_key_up(key, timestamp());
	}

	void on_mouse_key_down(native_window * window_body, xng_mouse_key key) override
	{
		m_inputHandler->on_mouse_key_down(key, timestamp());
	}

	void on_mouse_key_up(native_window * window_body, xng_mouse_key key) override
	{
		m_inputHandler->on_mouse_key_up(key, timestamp());
	}

	void on_mouse_move(native_window * window_body, const xng::math::uint2 & position) override
	{
		m_inputHandler->on_mouse_move(position, timestamp());
	}

	void on_mouse_wheel(native_window * window_body, int32_t wheel) override
	{
		m_inputHandler->on_mouse_wheel(wheel, timestamp());
	}

	void on_focus(native_window * window_body) override
	{
		m_inputHandler->on_focus_change(timestamp());
	}

	void on_unfocus(native_window * window_body) override
	{
		m_inputHandler->on_focus_change(timestamp());
	}

private:

	input_handler * m_inputHandler;
};

struct game_handled_input_observer :
	public keyboard_observer,
	public mouse_observer
{
	bool on_keyboard_key_down(const keyboard * keyboard, xng_keyboard_key key) override
	{
		//XNG_LOG("Pressed key", XNG_LOG_STREAM() << key);
		return true;
	}

	bool on_keyboard_key_up(const keyboard * keyboard, xng_keyboard_key key, uint32_t millis) override
	{
		//XNG_LOG("Released key", XNG_LOG_STREAM() << key << " after " << millis << "ms.");
		return true;
	}

	bool on_mouse_key_down(const mouse * mouse, xng_mouse_key key) override
	{
		XNG_LOG("Pressed key", XNG_LOG_STREAM() << key);
		//XNG_LOG("Mouse position", XNG_LOG_STREAM() << mouse->get_position());
		return true;
	}

	bool on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis) override
	{
		//XNG_LOG("Held key", XNG_LOG_STREAM() << key);
		return true;
	}

	bool on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis) override
	{
		XNG_LOG("Released key", XNG_LOG_STREAM() << key << " after " << millis << "ms.");
		return true;
	}

	bool on_mouse_wheel(const mouse * mouse, int32_t wheel) override
	{
		//XNG_LOG("Mouse wheel", XNG_LOG_STREAM() << wheel);
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
	m_renderReady(false),
	m_renderScene(nullptr),
	m_renderGUI(nullptr),
	m_ticksPerSecond(30)
{
}

bool game::init(void)
{
	assert(!m_running && !m_rendering && "Init called while the game is still running.");

	m_window     = std::make_unique<native_window>();
	m_mainLoop   = std::make_unique<main_loop>();
	m_guiManager = std::make_unique<gui_manager>();

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

	m_guiManager.reset();
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

		m_renderReady = false;
		m_rendering   = true;
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

		while (!m_renderReady && m_rendering)
		{
			m_renderCV.wait(renderLock);
		}

		if (m_renderReady)
		{
			m_render->render(m_renderScene, m_renderGUI);
			m_renderReady = false;
			m_renderCV.notify_all();
		}
	}
}

void game::game_loop(void)
{
	const uint32_t TicksPerSecond = m_ticksPerSecond;
	const uint32_t MaxFrameSkip   = 10;

	const uint64_t SkipTicks      = from_seconds(1.0 / TicksPerSecond);
	const float    TickSeconds    = 1.f / TicksPerSecond;

	const high_resolution_timestamp GameStart = timestamp();
	uint64_t nextTick = GameStart;

	scene * currentScene = nullptr;
	
	m_inputHandler.mouse().add_observer(m_guiManager.get());

	game_handled_input_observer inputObserver;

	m_inputHandler.keyboard().add_observer(&inputObserver);
	m_inputHandler.mouse().add_observer(&inputObserver);

	game_window_input_observer inputWindowObserver(&m_inputHandler);
	m_window->add_observer(&inputWindowObserver);

	while (m_running)
	{
		bool gameLogicUpdated = false;

		for (uint32_t loops = 0;
			timestamp() > nextTick && loops < MaxFrameSkip;
			++loops)
		{
			gameLogicUpdated = true;

			m_inputHandler.dispatch(nextTick);

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

			// TODO: Use events to set GUI size
			m_guiManager->set_size(m_window->get_client_size());

			//XNG_LOG("Update", XNG_LOG_STREAM() << "Game time " << nextTick << ", current time " << timestamp() << ", difference " << to_seconds<float>(timestamp() - nextTick));

			nextTick += SkipTicks;
		}

		// Feed the rendering thread a new scene (TODO: interpolation)

		if (m_rendering)
		{
			std::unique_lock<std::mutex> renderLock(m_renderMutex);

			while (m_renderReady && m_rendering)
			{
				m_renderCV.wait(renderLock);
			}

			if (!m_renderReady)
			{
				// If an update happened, clone the scene to be rendered and feed
				// it to the rendering thread, otherwise render the previous scene

				if (gameLogicUpdated)
				{
					if (m_renderScene)
					{
						xng_delete m_renderScene;
					}

					if (m_renderGUI)
					{
						xng_delete m_renderGUI;
					}

					m_renderScene = currentScene ? currentScene->clone() : nullptr;
					m_renderGUI   = m_guiManager->clone();
				}
				
				m_renderReady = true;
				m_renderCV.notify_all();
			}
		}
	}

	m_window->remove_observer(&inputWindowObserver);
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

gui_manager * game::get_gui_manager(void) const
{
	return m_guiManager.get();
}

void game::set_ticks_per_second(uint32_t ticks)
{
	m_ticksPerSecond = ticks;
}

uint32_t game::get_ticks_per_second(void) const
{
	return m_ticksPerSecond;
}

const input_handler * game::get_input_handler(void) const
{
	return &m_inputHandler;
}