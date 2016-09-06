#include <xng/graphics/scene_graph_node.hpp>
#include <xng/graphics/scene_graph.hpp>

#include <cassert>
#include <algorithm>
#include <iterator>

using namespace xng::graphics;
using namespace xng::math;

scene_graph_node::~scene_graph_node(void)
{
	for (auto child : m_children)
	{
		child->on_parent_destruction();
	}

	if (m_parent)
	{
		m_parent->on_child_destruction(this);
	}
}

scene_graph * scene_graph_node::get_scene_graph(void)
{
	return m_graph;
}

void scene_graph_node::set_scene_graph(scene_graph * graph)
{
	m_graph = graph;
}

scene_graph_node * scene_graph_node::get_root(void)
{
	return m_graph->get_root();
}

scene_graph_node * scene_graph_node::find_child_by_name(const char * name)
{
	auto it = std::find_if(m_children.begin(), m_children.end(), [=](scene_graph_node * n)
	{
		return (strcmp(n->get_name(), name) == 0);
	});

	return it == m_children.end() ? nullptr : *it;
}

scene_graph_node * scene_graph_node::find_child_by_type(xng_scene_graph_node_type type)
{
	auto it = std::find_if(m_children.begin(), m_children.end(), [=](scene_graph_node * n)
	{
		return (n->get_type() == type);
	});

	return it == m_children.end() ? nullptr : *it;
}

std::vector<scene_graph_node*> scene_graph_node::find_children_by_name(const char * name)
{
	std::vector<scene_graph_node*> v;

	std::copy_if(m_children.begin(), m_children.end(), std::back_inserter(v),
		[=](scene_graph_node * n)
	{
		return (strcmp(n->get_name(), name) == 0);
	});

	return v;
}

std::vector<scene_graph_node*> scene_graph_node::find_children_by_type(xng_scene_graph_node_type type)
{
	std::vector<scene_graph_node*> v;

	std::copy_if(m_children.begin(), m_children.end(), std::back_inserter(v),
		[=](scene_graph_node * n)
	{
		return (n->get_type() == type);
	});

	return v;
}

bool scene_graph_node::is_ancestor(scene_graph_node * node) const
{
	return m_parent != nullptr && (m_parent == node || m_parent->is_ancestor(node));
}

bool scene_graph_node::is_descendant(scene_graph_node * node) const
{
	for (scene_graph_node * child : m_children)
	{
		if (child == node)
		{
			return true;
		}
		return child->is_descendant(node);
	}
	return false;
}

scene_graph_node * scene_graph_node::get_parent(void) const
{
	return m_parent;
}

void scene_graph_node::set_parent(scene_graph_node * parent)
{
	float4x4 oldMatrix = get_global_matrix();

	scene_graph_node * oldParent = m_parent;

	if (m_parent)
	{
		auto it = std::find(oldParent->m_children.begin(), oldParent->m_children.end(), this);

		if (it != oldParent->m_children.end())
		{
			oldParent->m_children.erase(it);
		}
	}

	switch_parent(parent);

	if (parent)
	{
		parent->m_children.push_back(this);
	}
}

void scene_graph_node::set_parent_internal(scene_graph_node * node)
{
	m_parent = node;
}

void scene_graph_node::update(void)
{
	if (was_moved())
	{
		float4x4 oldTransform = get_global_matrix_internal();
		float4x4 newTransform = get_global_matrix();

		update_impl();

		/*for (scene_graph_node_listener * listener : m_listeners)
		{
			listener->on_scene_graph_node_move(this, oldTransform, newTransform);
		}*/

		set_moved(false);
	}

	for (scene_graph_node * n : m_children) n->update();
}

void scene_graph_node::on_parent_destruction(void)
{
	m_parent = nullptr;
	XNG_DELETE this;
}

void scene_graph_node::on_child_destruction(scene_graph_node * child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	assert(it != m_children.end() && "Consistency error: parent children list doesn't contain the child node.");
	m_children.erase(it);
}

void scene_graph_node::clear_children(void)
{
	for (auto child : m_children)
	{
		XNG_DELETE child;
	}

	m_children.clear();
}

const char * scene_graph_node::get_name(void) const
{
	return m_name.c_str();
}

void scene_graph_node::set_name(const char * name)
{
	m_name = name;
}

xng_scene_graph_node_type scene_graph_node::get_type(void) const
{
	return m_type;
}

void scene_graph_node::on_parent_destruction_internal(void)
{
	m_parent = nullptr;
	XNG_DELETE this;
}

void scene_graph_node::on_child_destruction_internal(scene_graph_node * child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	assert(it != m_children.end() && "Consistency error: parent children list doesn't contain the child node.");
	m_children.erase(it);
}