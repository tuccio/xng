#include <xng/dx11/default_samplers.hpp>

using namespace xng::dx11;

default_samplers::default_samplers(void) :
    m_anisotropy(16) {}

default_samplers::~default_samplers(void)
{
    shutdown();
}

bool default_samplers::init(ID3D11Device * device)
{
    float black[4] = { 0.f };

    D3D11_SAMPLER_DESC desc[] = {
        // PointWrap
        CD3D11_SAMPLER_DESC(D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX),
        // PointClamp
        CD3D11_SAMPLER_DESC(D3D11_FILTER_MIN_MAG_MIP_POINT,
            D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX),
        // LinearWrap
        CD3D11_SAMPLER_DESC(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX),
        // LinearClamp
        CD3D11_SAMPLER_DESC(D3D11_FILTER_MIN_MAG_MIP_LINEAR,
            D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX),
        // AnisotropicWrap
        CD3D11_SAMPLER_DESC(D3D11_FILTER_ANISOTROPIC,
            D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX),
        // AnisotropicClamp
        CD3D11_SAMPLER_DESC(D3D11_FILTER_ANISOTROPIC,
            D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
            0.f, m_anisotropy, D3D11_COMPARISON_NEVER, black, 0.f, FLT_MAX)

    };

    for (int i = 0; i < m_samplers.size(); ++i)
    {
        if (XNG_HR_FAILED(device->CreateSamplerState(desc + i, m_samplers[i].reset_and_get_address())))
        {
            return false;
        }
    }

    return true;
}

void default_samplers::shutdown(void)
{
    for (auto & s : m_samplers)
    {
        s.reset();
    }
}

void default_samplers::bind_vertex_shader(ID3D11DeviceContext * context)
{
    context->VSSetSamplers(10, m_samplers.size(), reinterpret_cast<ID3D11SamplerState * const *>(&m_samplers[0]));
}

void default_samplers::bind_pixel_shader(ID3D11DeviceContext * context)
{
    context->PSSetSamplers(10, m_samplers.size(), reinterpret_cast<ID3D11SamplerState * const *>(&m_samplers[0]));
}

void default_samplers::bind_compute_shader(ID3D11DeviceContext * context)
{
    context->CSSetSamplers(10, m_samplers.size(), reinterpret_cast<ID3D11SamplerState * const *>(&m_samplers[0]));
}