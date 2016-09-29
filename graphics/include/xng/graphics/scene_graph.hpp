#pragma once

#include <xng/core.hpp>

#include <xng/graphics/scene_graph_node.hpp>
#include <xng/graphics/scene_graph_group.hpp>
#include <xng/graphics/scene_graph_light.hpp>
#include <xng/graphics/scene_graph_geometry.hpp>
#include <xng/graphics/scene_graph_camera.hpp>

#include <memory>
#include <vector>

namespace xng
{
	namespace graphics
	{
		class scene_graph;

		struct scene_graph_observer
		{
			virtual void on_create(scene_graph * sg, scene_graph_node * node) {}
			virtual void on_destroy(scene_graph * sg, scene_graph_node * node) {}
			virtual void on_move(scene_graph * sg, scene_graph_node * node, const math::float4x4 & lastTransform, const math::float4x4 & newTransform) {}
		};

		class scene_graph :
			public core::observable<scene_graph_observer>
		{

		public:

			scene_graph(void);
			scene_graph(scene_graph &&);

			scene_graph_node * get_root(void);

			template <typename Visitor>
			void visit(Visitor v)
			{
				visit_impl(m_root.get(), v);
			}

		private:

			template <typename Visitor>
			void visit_impl(scene_graph_node * n, Visitor v)
			{
				v(n);

				for (scene_graph_node * c : *n)
				{
					visit_impl(c, v);
				}
			}

		private:

			std::unique_ptr<scene_graph_node> m_root;

		};
	}
}