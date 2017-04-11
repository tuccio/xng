#pragma once

#include <xng/graphics/scene_graph.hpp>
#include <xng/graphics/scene_graph_node.hpp>

using namespace xng::graphics;

scene_graph::scene_graph(void) :
    m_nextID(0)
{
    m_root = std::make_unique<scene_graph_group>(this);
}

scene_graph::scene_graph(scene_graph && rhs)
{
    m_root = std::move(rhs.m_root);
    visit([=](scene_graph_node * n) { n->set_scene_graph(this); });
}

scene_graph_node * scene_graph::get_root(void)
{
    return m_root.get();
}