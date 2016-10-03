#include <xng/graphics/scene_graph_light.hpp>

using namespace xng::graphics;
using namespace xng::math;

scene_graph_light::scene_graph_light(scene_graph_node * parent) :
	scene_graph_node(XNG_SCENE_GRAPH_LIGHT, parent, parent->get_scene_graph()) {}

bool scene_graph_light::is_static(void) const
{
	return m_static;
}

void scene_graph_light::set_static(bool s)
{
	m_static = s;
}

bool scene_graph_light::is_shadow_caster(void) const
{
	return m_shadowCaster;
}

void scene_graph_light::set_shadow_caster(bool shadowCaster)
{
	m_shadowCaster = shadowCaster;
}

const float3 & scene_graph_light::get_position(void) const
{
	return m_position;
}

void scene_graph_light::set_position(const float3 & position)
{
	m_position = position;
}

const float3 & scene_graph_light::get_direction(void) const
{
	return m_direction;
}

void scene_graph_light::set_direction(const float3 & direction)
{
	m_direction = direction;
}

xng_light_type scene_graph_light::get_light_type(void) const
{
	return m_type;
}

void scene_graph_light::set_light_type(xng_light_type type)
{
	m_type = type;
}

const float3 & scene_graph_light::get_color(void) const
{
	return m_color;
}

void scene_graph_light::set_color(const float3 & color)
{
	m_color = color;
}

float scene_graph_light::get_intensity(void) const
{
	return m_intensity;
}

void scene_graph_light::set_intensity(float intensity)
{
	m_intensity = intensity;
}

const float3 & scene_graph_light::get_ambient(void) const
{
	return m_ambient;
}

void scene_graph_light::set_ambient(const float3 & ambient)
{
	m_ambient = ambient;
}

float scene_graph_light::get_cutoff_angle(void) const
{
	return m_cutoff;
}

void scene_graph_light::set_cutoff_angle(float angle)
{
	m_cutoff = angle;
}