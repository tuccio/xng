#include <xng/graphics/scene_graph_light.hpp>

using namespace xng::graphics;
using namespace xng::math;

scene_graph_light::scene_graph_light(scene_graph_node * parent) :
    scene_graph_node(XNG_SCENE_GRAPH_LIGHT, parent, parent->get_scene_graph()),
    m_color(0),
    m_ambient(0),
    m_intensity(0),
    m_ambientIntensity(0),
    m_cutoff(pi() / 4) {}

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

float3 scene_graph_light::get_position(void)
{
    return get_global_translation();
}

void scene_graph_light::set_position(const float3 & position)
{
    set_global_translation(position);
}

float3 scene_graph_light::get_direction(void)
{
    return get_global_rotation() * float3(0, 0, 1);
}

void scene_graph_light::set_direction(const float3 & direction)
{
    const float3 forward = float3(0, 0, 1);

    float  cosAngle = dot(forward, direction);
    float3 rotAxis  = cross(forward, direction);

    float angle = std::acos(cosAngle);

    quaternion q(rotAxis, angle);

    set_global_rotation(q);
}

xng_light_type scene_graph_light::get_light_type(void) const
{
    return m_type;
}

void scene_graph_light::set_light_type(xng_light_type type)
{
    m_type = type;
}

const ubyte3 & scene_graph_light::get_color(void) const
{
    return m_color;
}

void scene_graph_light::set_color(const ubyte3 & color)
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

float3 scene_graph_light::get_luminance(void) const
{
    return (float3)m_color * (m_intensity / 255.f);
}

const ubyte3 & scene_graph_light::get_ambient_color(void) const
{
    return m_ambient;
}

void scene_graph_light::set_ambient_color(const ubyte3 & ambient)
{
    m_ambient = ambient;
}

float scene_graph_light::get_ambient_intensity(void) const
{
    return m_ambientIntensity;
}

void scene_graph_light::set_ambient_intensity(float intensity)
{
    m_ambientIntensity = intensity;
}

float3 scene_graph_light::get_ambient_luminance(void) const
{
    return (float3)m_ambient * (m_ambientIntensity / 255.f);
}

float scene_graph_light::get_cutoff_angle(void) const
{
    return m_cutoff;
}

void scene_graph_light::set_cutoff_angle(float angle)
{
    m_cutoff = angle;
}