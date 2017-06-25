#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/graphics/renderer_configuration.hpp>
#include <xng/graphics/scene.hpp>

namespace xng
{
    namespace dx11
    {
        class shadow_mapping
        {

        public:

            ~shadow_mapping(void);

            bool init(dx11_api_context * context);
            void shutdown(void);


            void render(ID3D11DeviceContext * deviceContext,
                        ID3D11DepthStencilView * dsv,
                        const math::float4x4 & lightMatrix,
                        const graphics::extracted_scene & extractedScene,
                        const math::uint2 & resolution,
                        const math::uint2 & scissorOffset,
                        const graphics::render_variables & rvars);


        private:

            typedef std::unordered_map<xng_culling_mode, com_ptr<ID3D11RasterizerState>> rasterizer_state_map;

            dx11_api_context *   m_apiContext;
            constant_buffer      m_cb;
            uber_shader          m_shader;
            rasterizer_state_map m_rasterizerState;

            com_ptr<ID3D11BlendState> m_blendState;

        private:

            ID3D11RasterizerState * get_rasterizer_state(xng_culling_mode cullMode);

        };
    }
}