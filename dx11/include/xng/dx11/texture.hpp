#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

namespace xng
{
    namespace dx11
    {
        class texture :
            public res::resource
        {

        public:

            struct load_data
            {
                load_data(ID3D11Device * device, ID3D11DeviceContext * context) :
                    device(device), context(context) {}

                ID3D11Device        * device;
                ID3D11DeviceContext * context;
            };

            static const char * resource_type;

            texture(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);

            XNG_INLINE ID3D11Texture2D * get_texture(void) const
            {
                return m_texture.get();
            }

            XNG_INLINE ID3D11ShaderResourceView * get_shader_resource_view(void) const
            {
                return m_srv.get();
            }

            bool create(
                ID3D11Device * device,
                ID3D11DeviceContext * context,
                graphics::image_ptr image,
                D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE,
                UINT mipmaps = 1,
                bool generateMipmaps = false);

            void clear(void);

        protected:

            bool   load_impl(const void * userdata) override;
            void   unload_impl(void) override;
            size_t calculate_size_impl(void) override;

        private:

            com_ptr<ID3D11Texture2D>          m_texture;
            com_ptr<ID3D11ShaderResourceView> m_srv;

            uint32_t m_width;
            uint32_t m_height;
            uint32_t m_mipmaps;
            DXGI_FORMAT m_format;

        };

        typedef res::resource_ptr<texture>           texture_ptr;
        typedef res::basic_resource_manager<texture> texture_manager;
    }

    

}