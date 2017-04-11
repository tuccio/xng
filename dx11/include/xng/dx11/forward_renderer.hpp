#pragma once

#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/dx11/vertex_buffers_factory.hpp>

namespace xng
{
    namespace dx11
    {
        class forward_renderer 
        {

        public:

            bool init(dx11_api_context * context, const graphics::render_variables & rvars);
            void shutdown(void);

            void update_render_variables(const graphics::render_variables & rvars, const graphics::render_variables_updates & update);

            void depth_prepass(ID3D11DeviceContext * deviceContext,
                               ID3D11DepthStencilView * dsv,
                               const graphics::extracted_scene & extractedScene);

            void render(ID3D11DeviceContext * deviceContext,
                        ID3D11RenderTargetView * rtv,
                        ID3D11DepthStencilView * dsv,
                        bool renderDepth,
                        const graphics::extracted_scene & extractedScene,
                        bool debugNormals);

            void reload_shaders(void);

        private:

            dx11_api_context * m_apiContext;
            graphics::render_variables m_rvars;

            com_ptr<ID3D11RasterizerState>   m_rasterizerState;
            com_ptr<ID3D11BlendState>        m_blendNoRTState;
            com_ptr<ID3D11DepthStencilState> m_depthCompareState;

            constant_buffer m_cbPerObjectDepth;
            constant_buffer m_cbPerObject;
            constant_buffer m_cbPerFrame;
            
            com_ptr<ID3D11Buffer>             m_sbLights;
            com_ptr<ID3D11ShaderResourceView> m_srvLights;
            uint32_t                          m_maxLights;

            vertex_buffers_factory m_vbFactory;
            uber_shader            m_program;

        };
    }
}