#include <xng/scene/basic_scene_module.hpp>

using namespace xng::graphics;
using namespace xng::scene;

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

scene_ptr basic_scene_module::create_scene(const char * name)
{
    scene_ptr scene = find_scene(name);

    if (!scene)
    {
        basic_scene * newScene = xng_new basic_scene();
        scene = newScene;
        m_scenes[name] = newScene;
        return scene;
    }
    else
    {
        return nullptr;
    }
}

bool basic_scene_module::destroy_scene(const char * name)
{
    auto it = m_scenes.find(name);

    if (it != m_scenes.end())
    {
        if (m_activeScene.get() == it->second.get())
        {
            m_activeScene.reset();
        }

        m_scenes.erase(it);
        return true;
    }
    else
    {
        return false;
    }
}

scene_ptr basic_scene_module::find_scene(const char * name)
{
    auto it = m_scenes.find(name);
    return it != m_scenes.end() ? it->second.get() : nullptr;
}

scene_ptr basic_scene_module::get_active_scene(void)
{
    return m_activeScene;
}

void basic_scene_module::set_active_scene(const scene_ptr & scene)
{
    m_activeScene = scene;
}