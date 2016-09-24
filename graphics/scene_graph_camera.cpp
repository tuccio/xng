#include <xng/graphics/scene_graph_camera.hpp>

using namespace xng::graphics;

scene_graph_camera::scene_graph_camera(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_CAMERA, parent, parent->get_scene_graph()) {}