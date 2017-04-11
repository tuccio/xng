#pragma once

#include <xng/dx11/render_resource.hpp>
#include <xng/core.hpp>

#include <map>
#include <unordered_map>

namespace xng
{
    namespace dx11
    {
        namespace detail
        {
            struct render_resource_wrapper
            {
                D3D11_TEXTURE2D_DESC description;
                uint32_t             id;
                bool                 usageFlag;
            };            
        }

        class render_resource_manager;

        class render_resource_handle :
            public core::refcounted_obj<render_resource_handle>
        {

        public:

            render_resource_handle(void);
            ~render_resource_handle(void);

            const render_resource * get_render_resource(void) const;

        private:

            render_resource_manager * m_manager;
            uint32_t m_id;

            render_resource_handle(render_resource_manager * manager, uint32_t id);

            friend class render_resource_manager;

        };

        typedef core::refcounted_ptr<render_resource_handle> render_resource_handle_ptr;

        class render_resource_manager
        {

        public:

            render_resource_handle_ptr get_texture_2d(
                ID3D11Device * device,
                const D3D11_TEXTURE2D_DESC & desc,
                const D3D11_RENDER_TARGET_VIEW_DESC * rtvDesc = nullptr,
                const D3D11_SHADER_RESOURCE_VIEW_DESC * srvDesc = nullptr,
                const D3D11_UNORDERED_ACCESS_VIEW_DESC * uavDesc = nullptr,
                const D3D11_DEPTH_STENCIL_VIEW_DESC * dsvDesc = nullptr);

            void clear(void);

        private:

            std::multimap<uint64_t, detail::render_resource_wrapper*>                     m_descriptionMap;
            std::vector<std::pair<render_resource*, detail::render_resource_wrapper*>> m_resources;

            detail::render_resource_wrapper * find_texture_2d(const D3D11_TEXTURE2D_DESC & description);
            detail::render_resource_wrapper * create_texture_2d(ID3D11Device * device, const D3D11_TEXTURE2D_DESC & description);

            void release(uint32_t id);
            const render_resource * get_render_resource(uint32_t id) const;

            friend class render_resource_handle;

        };
    }
}