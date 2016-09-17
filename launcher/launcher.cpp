#include <xng/os.hpp>
#include <xng/res.hpp>
#include <xng/engine.hpp>
#include <xng/graphics.hpp>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	std::unique_ptr<xng::os::debug_ostream> logStream(xng_new xng::os::debug_ostream);
	std::unique_ptr<xng::core::logger> logger(xng_new xng::core::logger(logStream.get()));

	std::unique_ptr<xng::res::resource_factory> resourceFactory(xng_new xng::res::resource_factory);

	std::unique_ptr<xng::graphics::mesh_manager>  meshManager(xng_new xng::graphics::mesh_manager);
	std::unique_ptr<xng::graphics::image_manager> imageManager(xng_new xng::graphics::image_manager);
	std::unique_ptr<xng::graphics::font_manager>  fontManager(xng_new xng::graphics::font_manager);

	resourceFactory->register_manager(meshManager.get());
	resourceFactory->register_manager(imageManager.get());
	resourceFactory->register_manager(fontManager.get());

	std::unique_ptr<xng::engine::module_manager> modules(xng_new xng::engine::module_manager);

	modules->register_shared_library("xngdx11");
	modules->register_shared_library("xnggl");
	modules->register_shared_library("xngeditor");
	modules->register_shared_library("xngscene");

	xng::engine::module_factory * renderFactory  = modules->find_module_by_type(XNG_MODULE_TYPE_RENDER);
	xng::engine::module_factory * runtimeFactory = modules->find_module_by_type(XNG_MODULE_TYPE_RUNTIME);

	std::unique_ptr<xng::engine::game> game(xng_new xng::engine::game);

	game->set_render_module(dynamic_cast<xng::engine::render_module*>(renderFactory->create()));
	game->set_runtime_module(dynamic_cast<xng::engine::runtime_module*>(runtimeFactory->create()));

	if (game->init())
	{
		game->run();
	}

	game->shutdown();

	// Cleanup

	resourceFactory.reset();

	fontManager->clear();
	meshManager->clear();
	imageManager->clear();

	game->clear();

	XNG_DEBUG_NEW_REPORT();

	return 0;
}