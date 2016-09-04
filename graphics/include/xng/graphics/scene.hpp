#pragma once

#include <xng/graphics/scene_graph.hpp>
#include <xng/graphics/camera.hpp>
#include <vector>

namespace xng
{
	namespace graphics
	{
		class scene
		{

		public:

			typedef std::vector<scene_graph_geometry*> geometry_vector;
			typedef std::vector<scene_graph_camera*>   camera_vector;

			virtual ~scene(void) {}

			virtual scene_graph * get_scene_graph(void) = 0;

			virtual geometry_vector frustum_culling_static(const camera * cam) = 0;
			virtual geometry_vector frustum_culling_dynamic(const camera * cam) = 0;

			virtual geometry_vector get_geometry_nodes(void) = 0;
			virtual camera_vector   get_camera_nodes(void) = 0;

			virtual scene_graph_camera * get_active_camera(void) = 0;
			virtual void set_active_camera(scene_graph_camera * camera) = 0;

			virtual void update(void) = 0;

			virtual scene * clone(void) = 0;

		};
	}
}