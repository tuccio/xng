#include <xng/graphics/material.hpp>

using namespace xng::math;
using namespace xng::graphics;
using namespace xng::res;

const char * material::resource_type = "material";

material::material(const char * name,
    const resource_parameters & params,
    resource_loader_ptr loader,
    resource_manager * owner) :
    resource(name, params, loader, owner) {}

const float3 & material::get_base_color(void) const
{
    return m_baseColor;
}

void material::set_base_color(const float3 & baseColor)
{
    m_baseColor;
}

image_ptr material::get_base_texture(void) const
{
    return m_baseTexture;
}

void material::set_base_texture(const image_ptr & img)
{
    m_baseTexture = img;
}

image_ptr material::get_normal_map(void) const
{
    return m_normalMap;
}

void material::set_normal_map(const image_ptr & img)
{
    m_normalMap = img;
}

image_ptr material::get_specular_map(void) const
{
    return m_specularMap;
}

void material::set_specular_map(const image_ptr & specularMap)
{
    m_specularMap = specularMap;
}

bool material::load_impl(const void * userdata)
{
    // TODO: Read material from file

    if (m_baseTexture && !m_baseTexture->load())
    {
        return false;
    }

    if (m_specularMap && !m_specularMap->load())
    {
        return false;
    }

    if (m_normalMap && !m_normalMap->load())
    {
        return false;
    }

    return true;
}

void material::unload_impl(void)
{
    m_baseTexture.reset();
    m_specularMap.reset();
    m_normalMap.reset();
}

size_t material::calculate_size_impl(void)
{
    size_t size = 1;

    if (m_baseTexture && m_baseTexture->load())
    {
        size += m_baseTexture->get_size();
    }

    if (m_specularMap && m_specularMap->load())
    {
        size += m_specularMap->get_size();
    }

    if (m_normalMap && m_normalMap->load())
    {
        size += m_normalMap->get_size();
    }

    return size;
}

void material::set_default(void)
{
    m_alpha       = 1.f;
    m_baseColor   = float3(.75f, .64f, .82f);
    m_metallic    = 0.f;
    m_fresnelZero = .004f;
    m_roughness   = .6f;
    m_subsurface  = 0.f;
    m_specular    = 0.57f;

    m_baseTexture.reset();
    m_specularMap.reset();
    m_normalMap.reset();
}