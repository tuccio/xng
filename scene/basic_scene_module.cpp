#include <xng/scene/basic_scene_module.hpp>

using namespace xng::graphics;
using namespace xng::scene;

using scene_base = xng::graphics::scene;

const char *          basic_scene_module::module_name        = "xngbasicscene";
const char *          basic_scene_module::module_description = "Scene module composed with a simple scene graph and no acceleration structures.";
const xng_module_type basic_scene_module::module_type        = XNG_MODULE_TYPE_SCENE;

basic_scene_module::basic_scene_module(void) :
	m_activeScene(nullptr),
	m_initialized(false) {}

bool basic_scene_module::init(void)
{
	m_initialized = true;
	return m_initialized;
}

void basic_scene_module::shutdown(void)
{
	m_initialized = false;
	m_scenes.clear();
}

bool basic_scene_module::is_initialized(void) const
{
	return m_initialized;
}

scene_base * basic_scene_module::create_scene(const char * name)
{
	scene_base * scene = find_scene(name);

	if (!scene)
	{
		std::unique_ptr<basic_scene> newScene = std::make_unique<basic_scene>();
		scene = newScene.get();
		m_scenes[name] = std::move(newScene);
		return scene;
	}
	else
	{
		return nullptr;
	}
}

scene_base * basic_scene_module::find_scene(const char * name)
{
	auto it = m_scenes.find(name);
	return it != m_scenes.end() ? it->second.get() : nullptr;
}

scene_base * basic_scene_module::get_active_scene(void)
{
	return m_activeScene;
}

void basic_scene_module::set_active_scene(scene_base * scene)
{
	m_activeScene = dynamic_cast<basic_scene*>(scene);
}