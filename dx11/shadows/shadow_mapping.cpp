#include <xng/dx11/shadows/shadow_mapping.hpp>
#include <xng/dx11/gpu_mesh.hpp>

#include <xng/res/resource_factory.hpp>

#include <boost/range/join.hpp>

using namespace xng::dx11;
using namespace xng::math;
using namespace xng::graphics;
using namespace xng::res;

shadow_mapping::~shadow_mapping(void)
{
    shutdown();
}

bool shadow_mapping::init(dx11_api_context * context)
{
    m_apiContext = context;

    D3D11_BLEND_DESC blendNoRTDesc = {};

    m_shader.set_ilv_functor([](D3D11_INPUT_ELEMENT_DESC * inputElementDesc) { return 0; });

    return m_cb.create<float4x4>(context->get_device()) &&
           m_shader.preprocess(XNG_DX11_SHADER_FILE("shadows/shadow_mapping.xhlsl")) &&
           !XNG_HR_FAILED(context->get_device()->CreateBlendState(&blendNoRTDesc, &m_blendState));
}

void shadow_mapping::shutdown(void)
{
    m_cb.reset();
}

void shadow_mapping::render(ID3D11DeviceContext * deviceContext,
                            ID3D11DepthStencilView * dsv,
                            const float4x4 & lightMatrix,
                            const extracted_scene & extractedScene,
                            const uint2 & resolution,
                            const uint2 & scissorOffset,
                            const render_variables & rvars)
{
    deviceContext->RSSetViewports(1, &CD3D11_VIEWPORT(scissorOffset.x, scissorOffset.y, resolution.x, resolution.y));
    deviceContext->RSSetState(get_rasterizer_state(rvars.shadow_map_culling_mode));

    deviceContext->OMSetBlendState(m_blendState.get(), nullptr, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(nullptr, 0);
    deviceContext->OMSetRenderTargets(0, nullptr, dsv);

    deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);

    deviceContext->VSSetConstantBuffers(0, 1, &m_cb);

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    gpu_mesh::load_data meshLoadData = { m_apiContext->get_device() };

    shader_program program = m_shader.compile(m_apiContext->get_device(), "shadow_mapping");
    program.use(deviceContext);

    deviceContext->PSSetShader(nullptr, nullptr, 0);

    auto & renderables = extractedScene.get_renderables();

    auto & dynamicGeometry = extractedScene.get_frustum_culling_dynamic();
    auto & staticGeometry  = extractedScene.get_frustum_culling_static();

    for (auto renderableIndex : boost::range::join(staticGeometry, dynamicGeometry))
    {
        auto & renderable = renderables[renderableIndex];

        gpu_mesh_ptr mesh = resource_factory::get_singleton()->create<gpu_mesh>("", resource_parameters(), resource_loader_ptr(), renderable.mesh);

        if (mesh && mesh->load(&meshLoadData))
        {
            // Fill constant buffer

            const float4x4 & modelMatrix = renderable.world;
            const float4x4 mvp = lightMatrix * modelMatrix;
            m_cb.write(deviceContext, &mvp);

            // Draw

            UINT strides[] = { mesh->get_positional_data_stride() };
            UINT offsets[] = { mesh->get_positional_data_offset() };

            ID3D11Buffer * buffers[] = { mesh->get_positional_data() };

            deviceContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
            deviceContext->IASetIndexBuffer(mesh->get_indices_data(), mesh->get_indices_format(), mesh->get_indices_offset());

            deviceContext->DrawIndexed(mesh->get_num_indices(), 0, 0);
        }
    }

    program.dispose(deviceContext);
}

ID3D11RasterizerState * shadow_mapping::get_rasterizer_state(xng_culling_mode cullMode)
{
    ID3D11RasterizerState * state;

    auto it = m_rasterizerState.find(cullMode);

    if (it != m_rasterizerState.end())
    {
        state = it->second.get();
    }
    else
    {
        com_ptr<ID3D11RasterizerState> newState;

        CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);

        rasterizerDesc.FrontCounterClockwise = TRUE;

        switch (cullMode)
        {
        case XNG_CULLING_MODE_NONE:
            rasterizerDesc.CullMode = D3D11_CULL_NONE;
            break;
        default:
        case XNG_CULLING_MODE_FRONT:
            rasterizerDesc.CullMode = D3D11_CULL_FRONT;
            break;
        case XNG_CULLING_MODE_BACK:
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            break;
        }

        XNG_HR_CHECK(m_apiContext->get_device()->CreateRasterizerState(
            &rasterizerDesc, &newState));

        state = newState.get();
        m_rasterizerState.emplace(cullMode, std::move(newState));
    }

    return state;
}
