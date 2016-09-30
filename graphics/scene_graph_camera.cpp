#include <xng/graphics/scene_graph_camera.hpp>

using namespace xng::graphics;

scene_graph_camera::scene_graph_camera(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_CAMERA, parent, parent->get_scene_graph()) {}

void scene_graph_camera::update_impl(void)
{
	m_camera.set_orientation(get_global_rotation());
	m_camera.set_position(to_float3(get_global_translation()));
}