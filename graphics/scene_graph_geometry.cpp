#include <xng/graphics/scene_graph_geometry.hpp>

using namespace xng::graphics;

scene_graph_geometry::scene_graph_geometry(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_GEOMETRY, parent, parent->get_scene_graph()),
	m_static(false) {}

mesh_ptr scene_graph_geometry::get_mesh(void) const
{
	return m_mesh;
}

void scene_graph_geometry::set_mesh(mesh_ptr mesh)
{
	m_mesh = mesh;
}

bool scene_graph_geometry::is_static(void) const
{
	return m_static;
}

void scene_graph_geometry::set_static(bool s)
{
	m_static = s;
}

void scene_graph_geometry::update_impl(void)
{

}

scene_graph_geometry * scene_graph_geometry::clone(scene_graph_node * newParent, scene_graph * newSceneGraph)
{
	scene_graph_geometry * newNode = new scene_graph_geometry(*this);

	set_parent_internal(newParent);
	set_scene_graph(newSceneGraph);

	for (scene_graph_node * & node : newNode->m_children)
	{
		scene_graph_node * newChild = node->clone(newNode, newSceneGraph);
		node = newChild;
	}

	return newNode;
}