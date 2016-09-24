#pragma once

#include <xng/graphics/scene_graph.hpp>
#include <xng/graphics/camera.hpp>
#include <vector>

namespace xng
{
	namespace graphics
	{
		class extracted_scene;

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

			extracted_scene extract(void);

		};

		struct extracted_renderable
		{
			mesh_ptr       mesh;
			math::float4x4 world;
		};

		class extracted_scene
		{

		public:

			typedef std::vector<uint32_t> renderable_index_vector;
			typedef std::vector<extracted_renderable> renderable_vector;

			extracted_scene(void) = default;
			extracted_scene(const extracted_scene &) = default;
			extracted_scene(extracted_scene &&) = default;

			extracted_scene & operator= (const extracted_scene &) = default;
			extracted_scene & operator= (extracted_scene &&) = default;

			void clear(void);

			const camera * get_active_camera(void) const;

			const extracted_renderable * get_renderable(uint32_t index) const;

			const renderable_index_vector & frustum_culling_static(void) const;
			const renderable_index_vector & frustum_culling_dynamic(void) const;

		private:

			camera m_activeCamera;

			renderable_vector       m_renderables;
			renderable_index_vector m_visibleDynamicObjects;
			renderable_index_vector m_visibleStaticObjects;

			friend class scene;
		};
	}
}