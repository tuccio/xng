#pragma once

#include <xng/dx11/dx11_headers.hpp>

namespace xng
{
    namespace dx11
    {
        class render_resource
        {

        public:

            bool create(ID3D11Device * device, const D3D11_TEXTURE2D_DESC * desc);
            void clear(void);

            bool create_render_target_view(ID3D11Device * device, const D3D11_RENDER_TARGET_VIEW_DESC * desc = nullptr);
            bool create_unordered_access_view(ID3D11Device * device, const D3D11_UNORDERED_ACCESS_VIEW_DESC * desc = nullptr);
            bool create_shader_resource_view(ID3D11Device * device, const D3D11_SHADER_RESOURCE_VIEW_DESC * desc = nullptr);
            bool create_depth_stencil_view(ID3D11Device * device, const D3D11_DEPTH_STENCIL_VIEW_DESC * desc = nullptr);

            ID3D11Resource            * get_resource(void) const;
            ID3D11RenderTargetView    * get_render_target_view(void) const;
            ID3D11UnorderedAccessView * get_unordered_access_view(void) const;
            ID3D11ShaderResourceView  * get_shader_resource_view(void) const;
            ID3D11DepthStencilView    * get_depth_stencil_view(void) const;

        private:

            com_ptr<ID3D11Resource>            m_resource;
            com_ptr<ID3D11RenderTargetView>    m_rtv;
            com_ptr<ID3D11UnorderedAccessView> m_uav;
            com_ptr<ID3D11ShaderResourceView>  m_srv;
            com_ptr<ID3D11DepthStencilView>    m_dsv;

        };
    }
}