#include "editor_module.hpp"

#include <xng/os.hpp>
#include <xng/res.hpp>

using namespace xng::editor;
using namespace xng::engine;
using namespace xng::os;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

const char *          editor_module::module_name        = "xngeditor";
const char *          editor_module::module_description = "XNG Editor Module";
const xng_module_type editor_module::module_type        = XNG_MODULE_TYPE_RUNTIME;

void create_default_scene(scene_module * sceneModule);

bool editor_module::init(void)
{
	module_factory * sceneFactory = module_manager::get_singleton()->
		find_module_by_name("xngbasicscene");

	if (sceneFactory)
	{
		game * instance = game::get_singleton();

		native_window * window_body = instance->get_window();
		m_editor = std::make_unique<editor>(window_body);

		scene_module * sceneModule = dynamic_cast<scene_module*>(sceneFactory->create());

		if (sceneModule->init())
		{
			create_default_scene(sceneModule);
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
	scene * scene = game::get_singleton()->get_scene_module()->get_active_scene();

	{
		scene_graph_node * n = scene->get_scene_graph()->get_root()->find_child_by_name("triangle1");

		quaternion q1 = n->get_local_rotation();
		quaternion q2 = quaternion(float3(0, 1, 0), dt * XNG_PI / 12);

		quaternion q3 = normalize(q1 * q2);

		n->set_local_rotation(q3);
	}
	
	/*{
		camera * activeCamera = scene->get_active_camera()->get_camera();

		quaternion q1 = activeCamera->get_orientation();
		quaternion q2 = quaternion(float3(0, 1, 0), dt * XNG_PI / 4);

		quaternion q3 = normalize(q1 * q2);

		activeCamera->set_orientation(q3);
	}*/

	m_editor->update();
}

void create_default_scene(scene_module * sceneModule)
{

	scene * defaultScene = sceneModule->create_scene("default_scene");

	scene_graph_node     * sceneGraphRoot = defaultScene->get_scene_graph()->get_root();
	scene_graph_camera   * camera         = sceneGraphRoot->add_child<scene_graph_camera>();
	scene_graph_geometry * triangle1      = sceneGraphRoot->add_child<scene_graph_geometry>();
	scene_graph_geometry * triangle2      = sceneGraphRoot->add_child<scene_graph_geometry>();
	
	triangle1->set_name("triangle1");
	triangle2->set_name("triangle2");

	mesh_ptr triangleMesh = resource_factory::get_singleton()->create<mesh>(
		"mesh", "triangle", resource_parameters(),
		std::make_shared<dynamic_resource_loader>(
			[](resource * r)
	{
		mesh * m = static_cast<mesh*>(r);

		if (m->create(3, 1, XNG_MESH_STORAGE_NONE))
		{
			float vertices[] = {
				1.f, -1.f, 0.f,
				-1.f, -1.f, 0.f,
				0.f,  1.f, 0.f
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

	triangle1->set_mesh(triangleMesh);
	triangle2->set_mesh(triangleMesh);

	triangle2->set_local_translation(float3(5, 5, 0));

	camera->get_camera()->look_at(float3(0, 0, -15), float3(0, 1, 0), float3(0, 0, 0));

	defaultScene->set_active_camera(camera);
	sceneModule->set_active_scene(defaultScene);
}