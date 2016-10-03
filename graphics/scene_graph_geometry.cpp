#include <xng/graphics/scene_graph_geometry.hpp>

using namespace xng::graphics;

scene_graph_geometry::scene_graph_geometry(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_GEOMETRY, parent, parent->get_scene_graph()),
	m_static(false) {}

mesh_ptr scene_graph_geometry::get_mesh(void) const
{
	return m_mesh;
}

void scene_graph_geometry::set_mesh(mesh_ptr mesh)
{
	m_mesh = mesh;
}

bool scene_graph_geometry::is_static(void) const
{
	return m_static;
}

void scene_graph_geometry::set_static(bool s)
{
	m_static = s;
}

material_ptr scene_graph_geometry::get_material(void) const
{
	return m_material;
}

void scene_graph_geometry::set_material(material_ptr material)
{
	m_material = material;
}

void scene_graph_geometry::update_impl(void)
{
}