#include <xng/dx11/render_resource.hpp>

using namespace xng::dx11;

bool render_resource::create(ID3D11Device * device, const D3D11_TEXTURE2D_DESC * desc)
{
    com_ptr<ID3D11Texture2D> tex2d;
    
    return !XNG_HR_FAILED(device->CreateTexture2D(desc, nullptr, &tex2d)) &&
        tex2d.as<ID3D11Resource>(m_resource);
}

void render_resource::clear(void)
{
    m_resource.reset();
    m_rtv.reset();
    m_uav.reset();
    m_srv.reset();
    m_dsv.reset();
}

bool render_resource::create_render_target_view(ID3D11Device * device, const D3D11_RENDER_TARGET_VIEW_DESC * desc)
{
    return !XNG_HR_FAILED(device->CreateRenderTargetView(m_resource.get(), desc, m_rtv.reset_and_get_address()));
}

bool render_resource::create_unordered_access_view(ID3D11Device * device, const D3D11_UNORDERED_ACCESS_VIEW_DESC * desc)
{
    return !XNG_HR_FAILED(device->CreateUnorderedAccessView(m_resource.get(), desc, m_uav.reset_and_get_address()));
}

bool render_resource::create_shader_resource_view(ID3D11Device * device, const D3D11_SHADER_RESOURCE_VIEW_DESC * desc)
{
    return !XNG_HR_FAILED(device->CreateShaderResourceView(m_resource.get(), desc, m_srv.reset_and_get_address()));
}

bool render_resource::create_depth_stencil_view(ID3D11Device * device, const D3D11_DEPTH_STENCIL_VIEW_DESC * desc)
{
    return !XNG_HR_FAILED(device->CreateDepthStencilView(m_resource.get(), desc, m_dsv.reset_and_get_address()));
}

ID3D11Resource * render_resource::get_resource(void) const
{
    return m_resource.get();
}

ID3D11RenderTargetView * render_resource::get_render_target_view(void) const
{
    return m_rtv.get();
}

ID3D11UnorderedAccessView * render_resource::get_unordered_access_view(void) const
{
    return m_uav.get();
}

ID3D11ShaderResourceView * render_resource::get_shader_resource_view(void) const
{
    return m_srv.get();
}

ID3D11DepthStencilView * render_resource::get_depth_stencil_view(void) const
{
    return m_dsv.get();
}
