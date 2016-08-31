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
		class scene_graph
		{

		public:

			scene_graph(void);
			scene_graph(const scene_graph &) = delete;
			scene_graph(scene_graph &&);

			scene_graph_node * get_root(void);
			scene_graph * clone(void);

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