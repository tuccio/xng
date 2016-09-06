#include <xng/graphics/scene_graph_camera.hpp>

using namespace xng::graphics;

scene_graph_camera::scene_graph_camera(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_CAMERA, parent, parent->get_scene_graph()) {}

scene_graph_camera * scene_graph_camera::clone(scene_graph_node * newParent, scene_graph * newSceneGraph)
{
	scene_graph_camera * newNode = XNG_NEW scene_graph_camera(*this);

	set_parent_internal(newParent);
	set_scene_graph(newSceneGraph);

	for (scene_graph_node * & node : newNode->m_children)
	{
		scene_graph_node * newChild = node->clone(newNode, newSceneGraph);
		node = newChild;
	}

	return newNode;
}