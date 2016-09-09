#include <xng/os.hpp>
#include <xng/res.hpp>
#include <xng/engine.hpp>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	xng_new xng::core::logger(xng_new xng::os::debug_ostream);

	xng::res::resource_factory * resourceFactory = xng_new xng::res::resource_factory;

	resourceFactory->register_manager(xng_new xng::graphics::mesh_manager);

	xng::engine::module_manager * modules = xng_new xng::engine::module_manager;

	modules->register_shared_library("xngdx11");
	modules->register_shared_library("xnggl");
	modules->register_shared_library("xngeditor");
	modules->register_shared_library("xngscene");

	xng::engine::module_factory * renderFactory = modules->find_module_by_type(XNG_MODULE_TYPE_RENDER);
	xng::engine::module_factory * runtimeFactory = modules->find_module_by_type(XNG_MODULE_TYPE_RUNTIME);

	xng::engine::game * game = xng_new xng::engine::game;

	game->set_render_module(dynamic_cast<xng::engine::render_module*>(renderFactory->create()));
	game->set_runtime_module(dynamic_cast<xng::engine::runtime_module*>(runtimeFactory->create()));

	if (game->init())
	{
		game->run();
	}

	game->shutdown();

	XNG_DEBUG_NEW_REPORT();

	return 0;
}