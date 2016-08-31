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
	m_renderScene(nullptr)
{
}

bool game::init(void)
{
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

void game::run(void)
{
	game_window_observer windowObserver(m_mainLoop.get());
	m_window->add_observer(&windowObserver);

	m_mainLoop->set_idle_callback(
		[this]()
	{
		float dt = m_timer.get_elapsed_seconds();

		if (m_runtime)
		{
			m_runtime->update(dt);
		}

		if (m_render)
		{
			std::unique_lock<std::mutex> renderLock(m_renderMutex);
			m_render->render(nullptr, nullptr);
		}

		/*if (m_scene)
		{
			scene * currentScene = m_scene->get_active_scene();

			if (currentScene)
			{
				currentScene->update();

				if (m_render)
				{
					std::unique_lock<std::mutex> renderLock(m_renderMutex);
					m_renderScene = currentScene->clone();
				}
			}
		}	*/	
	});
	
	m_timer.get_elapsed_seconds();
	m_mainLoop->run();
	/*m_running         = true;
	m_renderingThread = std::thread(std::bind(&game::rendering_loop, this));

	os::high_resolution_timer<float> timer;

	while (m_running)
	{
		float dt = timer.get_elapsed_seconds();

		m_runtime->update(dt);

		scene * currentScene = m_scene->get_active_scene();

		currentScene->update();

		std::unique_lock<std::mutex> renderLock(m_renderMutex);
		m_renderScene = currentScene->clone();		
	}*/
}

void game::rendering_loop(void)
{
	std::unique_lock<std::mutex> renderLock(m_renderMutex);
	while (!m_renderScene) m_renderCV.wait(renderLock);

	m_render->render(m_renderScene, m_renderScene->get_active_camera()->get_camera());
}