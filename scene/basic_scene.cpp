#include <xng/scene/basic_scene.hpp>

using namespace xng::graphics;
using namespace xng::scene;

basic_scene::basic_scene(void) :
	m_activeCamera(nullptr) {}

scene_graph * basic_scene::get_scene_graph(void)
{
	return &m_sceneGraph;
}

scene::geometry_vector basic_scene::frustum_culling_static(const camera * cam)
{
	geometry_vector v;

	m_sceneGraph.visit([&](scene_graph_node * node)
	{
		if (node->get_type() == XNG_SCENE_GRAPH_GEOMETRY)
		{
			scene_graph_geometry * gnode = static_cast<scene_graph_geometry*>(node);

			if (gnode->is_static())
			{
				v.push_back(gnode);
			}
		}
	});

	return v;
}

scene::geometry_vector basic_scene::frustum_culling_dynamic(const camera * cam)
{
	geometry_vector v;

	m_sceneGraph.visit([&](scene_graph_node * node)
	{
		if (node->get_type() == XNG_SCENE_GRAPH_GEOMETRY)
		{
			scene_graph_geometry * gnode = static_cast<scene_graph_geometry*>(node);

			if (!gnode->is_static())
			{
				v.push_back(gnode);
			}
		}
	});

	return v;
}

scene_graph_camera * basic_scene::get_active_camera(void)
{
	return m_activeCamera;
}

void basic_scene::set_active_camera(scene_graph_camera * camera)
{
	m_activeCamera = camera;
}

void basic_scene::update(void)
{
	m_sceneGraph.get_root()->update();
}

scene * basic_scene::clone(void)
{
	return new basic_scene(*this);
}

scene::geometry_vector basic_scene::get_geometry_nodes(void)
{
	geometry_vector v;

	m_sceneGraph.visit([&](scene_graph_node * node)
	{
		if (node->get_type() == XNG_SCENE_GRAPH_GEOMETRY)
		{
			scene_graph_geometry * gnode = static_cast<scene_graph_geometry*>(node);
			v.push_back(gnode);
		}
	});

	return v;
}

scene::camera_vector basic_scene::get_camera_nodes(void)
{
	camera_vector v;

	m_sceneGraph.visit([&](scene_graph_node * node)
	{
		if (node->get_type() == XNG_SCENE_GRAPH_CAMERA)
		{
			scene_graph_camera * cnode = static_cast<scene_graph_camera*>(node);
			v.push_back(cnode);
		}
	});

	return v;
}