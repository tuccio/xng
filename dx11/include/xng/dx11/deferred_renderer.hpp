#pragma once

#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/dx11/vertex_buffers_factory.hpp>
#include <xng/dx11/shadows/shadow_map_renderer.hpp>

namespace xng
{
    namespace dx11
    {
        class deferred_renderer
        {
        public:

            static constexpr int MaxShadowMapsPerObject = 7;

        public:

            bool init(dx11_api_context * context, const graphics::render_variables & rvars);
            void shutdown(void);

            void update_render_variables(const graphics::render_variables & rvars, const graphics::render_variables_updates & update);

            void depth_prepass(ID3D11DeviceContext * deviceContext,
                               ID3D11DepthStencilView * dsv,
                               const graphics::extracted_scene & extractedScene);

            void render_gbuffer(ID3D11DeviceContext * deviceContext,
                                ID3D11DepthStencilView * dsv,
                                ID3D11RenderTargetView ** gbufferRTV,
                                ID3D11ShaderResourceView ** gbufferSRV,
                                bool renderDepth,
                                const graphics::extracted_scene & extractedScene);

            void render_shading(ID3D11DeviceContext * deviceContext,
                                ID3D11RenderTargetView * rtv,
                                ID3D11ShaderResourceView ** gbufferSRV,
                                const graphics::extracted_scene & extractedScene,
                                const std::unordered_map<int, std::vector<shadow_map>> & shadowMaps,
                                bool debugNormals);

            void reload_shaders(void);

        private:

            dx11_api_context * m_apiContext;
            graphics::render_variables m_rvars;

            com_ptr<ID3D11RasterizerState>   m_rasterizerState;
            com_ptr<ID3D11BlendState>        m_blendNoRTState;
            com_ptr<ID3D11BlendState>        m_blendAccumulate;
            com_ptr<ID3D11DepthStencilState> m_depthCompareState;

            constant_buffer m_cbPerObjectDepth;
            constant_buffer m_cbPerObject;
            constant_buffer m_cbPerLight;
            constant_buffer m_cbPerFrame;

            com_ptr<ID3D11Buffer>             m_sbLights;
            com_ptr<ID3D11ShaderResourceView> m_srvLights;
            uint32_t                          m_maxLights;

            vertex_buffers_factory m_vbFactory;
            uber_shader            m_gbufferProgram;
            uber_shader            m_shadingProgram;

        };
    }
}