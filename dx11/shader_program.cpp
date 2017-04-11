#include <xng/dx11/shader_program.hpp>

using namespace xng::dx11;

void shader_program::use(ID3D11DeviceContext * context) const
{
    if (m_inputLayout)
    {
        context->IASetInputLayout(m_inputLayout.get());
    }

    if (m_vertexShader)
    {
        context->VSSetShader(m_vertexShader.get(), nullptr, 0);
    }

    if (m_pixelShader)
    {
        context->PSSetShader(m_pixelShader.get(), nullptr, 0);
    }
}

void shader_program::dispose(ID3D11DeviceContext * context) const
{
    if (m_inputLayout)
    {
        context->IASetInputLayout(m_inputLayout.get());
    }

    if (m_vertexShader)
    {
        context->VSSetShader(nullptr, nullptr, 0);
    }

    if (m_pixelShader)
    {
        context->PSSetShader(nullptr, nullptr, 0);
    }
}