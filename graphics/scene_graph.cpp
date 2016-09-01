#pragma once

#include <xng/graphics/scene_graph.hpp>
#include <xng/graphics/scene_graph_node.hpp>

using namespace xng::graphics;

scene_graph::scene_graph(void)
{
	m_root = std::make_unique<scene_graph_group>(this);
}

scene_graph::scene_graph(const scene_graph & graph)
{
	if (graph.m_root)
	{
		m_root = std::unique_ptr<scene_graph_node>(graph.m_root->clone(nullptr, this));
		visit([=](scene_graph_node * n) { n->set_scene_graph(this); });
	}
}

scene_graph::scene_graph(scene_graph && rhs)
{
	m_root = std::move(rhs.m_root);
	visit([=](scene_graph_node * n) { n->set_scene_graph(this); });
}

scene_graph & scene_graph::operator= (const scene_graph & rhs)
{
	m_root.reset();

	scene_graph copy(rhs);
	m_root = std::move(copy.m_root);

	visit([=](scene_graph_node * n) { n->set_scene_graph(this); });

	return *this;
}

scene_graph_node * scene_graph::get_root(void)
{
	return m_root.get();
}