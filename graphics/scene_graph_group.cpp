#include <xng/graphics/scene_graph_group.hpp>

using namespace xng::graphics;

scene_graph_group * scene_graph_group::clone(scene_graph_node * newParent, scene_graph * newSceneGraph)
{
	scene_graph_group * newNode = new scene_graph_group(*this);

	set_parent_internal(newParent);
	set_scene_graph(newSceneGraph);

	for (scene_graph_node * & node : newNode->m_children)
	{
		scene_graph_node * newChild = node->clone(newNode, newSceneGraph);
		node = newChild;
	}	

	return newNode;
}