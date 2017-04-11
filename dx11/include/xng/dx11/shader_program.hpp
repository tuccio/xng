#pragma once

#include <xng/dx11/dx11_headers.hpp>

namespace xng
{
    namespace dx11
    {
        class shader_program
        {

        public:

            shader_program(void) = default;
            shader_program(const shader_program &) = default;
            shader_program(shader_program &&) = default;

            void use(ID3D11DeviceContext * context) const;
            void dispose(ID3D11DeviceContext * context) const;

            XNG_INLINE ID3D11VertexShader * get_vertex_shader(void) const
            {
                return m_vertexShader.get();
            }

            XNG_INLINE void set_vertex_shader(ID3D11VertexShader * shader)
            {
                m_vertexShader = shader;
            }

            XNG_INLINE ID3D11PixelShader * get_pixel_shader(void) const
            {
                return m_pixelShader.get();
            }

            XNG_INLINE void set_pixel_shader(ID3D11PixelShader * shader)
            {
                m_pixelShader = shader;
            }

            XNG_INLINE ID3D11InputLayout * get_input_layout(void) const
            {
                return m_inputLayout.get();
            }

            XNG_INLINE void set_input_layout(ID3D11InputLayout * layout)
            {
                m_inputLayout = layout;
            }

        private:

            com_ptr<ID3D11VertexShader> m_vertexShader;
            com_ptr<ID3D11InputLayout>  m_inputLayout;
            com_ptr<ID3D11PixelShader>  m_pixelShader;

        };
    }
}