#pragma once

#include <xng/graphics.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/render_resource_manager.hpp>
#include <xng/dx11/shadows/shadow_mapping.hpp>
#include <xng/math.hpp>
#include <unordered_map>

namespace xng
{
    namespace dx11
    {
        struct shadow_map
        {
            math::float4x4 matrix;
            render_resource_handle_ptr shadowMap;
        };

        class shadow_map_renderer
        {


        public:

            bool init(dx11_api_context * context, const graphics::render_variables & rvars);
            void shutdown(void);

            void update_render_variables(const graphics::render_variables & rvars, const graphics::render_variables_updates & updates);

            void render(ID3D11DeviceContext * immediateContext,
                        std::unordered_map<int, std::vector<shadow_map>> & shadowMaps,
                        render_resource_manager & renderResourceManager,
                        const graphics::extracted_scene & scene);

        private:

            dx11_api_context * m_context;
            shadow_mapping m_shadowMapping;
            graphics::render_variables m_rvars;

        };
    }
}