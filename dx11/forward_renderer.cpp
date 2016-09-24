#include <xng/dx11/forward_renderer.hpp>

#include <xng/dx11.hpp>
#include <xng/graphics/mesh.hpp>
#include <xng/dx11/gpu_mesh.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

struct alignas(16) __cbPerObject
{
	float4x4 viewMatrix;
	float4x4 viewProjectionMatrix;
	float4   color;
};

bool forward_renderer::init(dx11_api_context * context)
{
	m_apiContext = context;

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	ID3D11Device * device = m_apiContext ? m_apiContext->get_device() : nullptr;

	return device != nullptr &&
		m_program.preprocess(XNG_DX11_SHADER_FILE("test.xhlsl")) &&
		m_cbPerObject.create<__cbPerObject>(device) &&
		!XNG_HR_FAILED(device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState));
}

void forward_renderer::shutdown(void)
{
	m_apiContext = nullptr;
	m_rasterizerState.reset();
	m_cbPerObject.reset();
	m_program.clear();
	m_vbFactory.clear();
}

void forward_renderer::render(ID3D11DeviceContext * deviceContext, const extracted_scene & scene)
{
	ID3D11Device * device  = m_apiContext->get_device();

	float clearColor[4] = { 0 };

	ID3D11RenderTargetView * backBuffer = m_apiContext->get_back_buffer_rtv();

	deviceContext->ClearRenderTargetView(backBuffer, clearColor);
	deviceContext->RSSetViewports(1, &CD3D11_VIEWPORT(0.f, 0.f, m_rvars.render_resolution.x, m_rvars.render_resolution.y));

	float4 colors[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 }
	};

	deviceContext->RSSetState(m_rasterizerState.get());
	deviceContext->OMSetRenderTargets(1, &backBuffer, nullptr);

	const camera * camera = scene.get_active_camera();

	const float4x4 & viewMatrix = camera->get_directx_view_matrix();
	const float4x4 & projectionMatrix = camera->get_directx_projection_matrix();

	float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	auto & dynamicGeometry = scene.frustum_culling_dynamic();

	shader_program program = m_program.compile(m_apiContext->get_device());
	program.use(deviceContext);

	deviceContext->VSSetConstantBuffers(0, 1, &m_cbPerObject);
	deviceContext->PSSetConstantBuffers(0, 1, &m_cbPerObject);

	int ColorIndex = 0;

	gpu_mesh::load_data meshLoadData = { m_apiContext->get_device() };

	for (auto renderableIndex : dynamicGeometry)
	{
		auto renderable = scene.get_renderable(renderableIndex);

		gpu_mesh_ptr m = resource_factory::get_singleton()->create<gpu_mesh>("", resource_parameters(), resource_loader_ptr(), renderable->mesh);

		if (m && m->load(&meshLoadData))
		{
			const float4x4 & modelMatrix = renderable->world;

			__cbPerObject bufferPerObjectData = {
				viewMatrix * modelMatrix,
				viewProjectionMatrix * modelMatrix,
				colors[ColorIndex++ % 3]
			};

			m_cbPerObject.write(deviceContext, &bufferPerObjectData);

			UINT strides[] = { m->get_positional_data_stride(), m->get_non_positional_data_stride() };
			UINT offsets[] = { m->get_positional_data_offset(), m->get_non_positional_data_offset() };

			ID3D11Buffer * buffers[] = { m->get_positional_data(), m->get_non_positional_data() };

			deviceContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
			deviceContext->IASetIndexBuffer(m->get_indices_data(), m->get_indices_format(), m->get_indices_offset());
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			deviceContext->DrawIndexed(m->get_num_indices(), 0, 0);
		}
	}

	program.dispose(deviceContext);
}

void forward_renderer::update_render_variables(const render_variables & rvars, const render_variables_updates & update)
{
	m_rvars = rvars;
}