#include "editor_module.hpp"

#include <xng/os.hpp>
#include <xng/res.hpp>

using namespace xng::editor;
using namespace xng::engine;
using namespace xng::os;
using namespace xng::graphics;
using namespace xng::res;

const char *          editor_module::module_name        = "xngeditor";
const char *          editor_module::module_description = "XNG Editor Module";
const xng_module_type editor_module::module_type        = XNG_MODULE_TYPE_RUNTIME;

bool editor_module::init(void)
{
	module_factory * sceneFactory = module_manager::get_singleton()->
		find_module_by_name("xngbasicscene");

	if (sceneFactory)
	{
		game * instance = game::get_singleton();

		native_window * window = instance->get_window();
		m_editor = std::make_unique<editor>(window);

		scene_module * sceneModule = dynamic_cast<scene_module*>(sceneFactory->create());

		if (sceneModule->init())
		{
			scene * defaultScene = sceneModule->create_scene("default_scene");

			scene_graph_geometry * triangle = defaultScene->get_scene_graph()->get_root()->add_child<scene_graph_geometry>();

			mesh_ptr triangleMesh = resource_factory::get_singleton()->create<mesh>(
				"mesh", "triangle", resource_parameters(),
				std::make_shared<dynamic_resource_loader>(
					[](resource * r)
			{
				mesh * m = static_cast<mesh*>(r);

				if (m->create(3, 1, XNG_MESH_STORAGE_NONE))
				{
					float vertices[] = {
						-1.f, -1.f, 0.f,
						1.f, -1.f, 0.f,
						0.f,  1.f, 0.f,
					};

					uint32_t indices[] = {
						0, 1, 2
					};

					memcpy(m->get_vertices(), vertices, sizeof(vertices));
					memcpy(m->get_indices(), indices, sizeof(indices));

					return true;
				}

				return false;
			},
					[](resource * r)
			{
				static_cast<mesh*>(r)->clear();
			}
			));

			triangle->set_mesh(triangleMesh);

			sceneModule->set_active_scene(defaultScene);
		}

		instance->set_scene_module(sceneModule);

		return true;
	}

	return false;
}

void editor_module::shutdown(void)
{
	m_editor.reset();
}

bool editor_module::is_initialized(void) const
{
	return (bool) m_editor;
}

void editor_module::update(float dt)
{
	m_editor->update();
}