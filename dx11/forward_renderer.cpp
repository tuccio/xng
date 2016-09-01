#include <xng/dx11/forward_renderer.hpp>

#include <xng/dx11.hpp>
#include <xng/graphics/mesh.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

bool forward_renderer::init(dx11_api_context * context)
{
	m_apiContext = context;

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	rasterizerDesc.FrontCounterClockwise = TRUE;

	ID3D11Device * device = m_apiContext ? m_apiContext->get_device() : nullptr;

	return device != nullptr &&
		m_program.preprocess(XNG_DX11_SHADER_FILE("test.xhlsl")) &&
		m_cbPerObject.create<float4>(device) &&
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

void forward_renderer::render(scene * scene, const camera * camera)
{
	ID3D11Device        * device  = m_apiContext->get_device();
	ID3D11DeviceContext * context = m_apiContext->get_immediate_context();

	float clearColor[4] = { 0 };

	ID3D11RenderTargetView * backBuffer = m_apiContext->get_back_buffer_rtv();

	context->ClearRenderTargetView(backBuffer, clearColor);
	context->RSSetViewports(1, &CD3D11_VIEWPORT(0.f, 0.f, m_rvars.render_resolution.x, m_rvars.render_resolution.y));

	float4 colors[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 }
	};

	context->RSSetState(m_rasterizerState.get());
	context->OMSetRenderTargets(1, &backBuffer, nullptr);
	
	auto geometry = scene->get_geometry_nodes();

	shader_program program = m_program.compile(m_apiContext->get_device());
	program.use(context);

	context->PSSetConstantBuffers(0, 1, &m_cbPerObject);

	for (auto gNode : geometry)
	{

		gpu_mesh_ptr m = make_gpu_mesh(gNode->get_mesh());

		if (m && m->load())
		{

			m_cbPerObject.write(context, &colors[m->get_id() % 3]);

			UINT strides[] = { m->get_positional_data_stride(), m->get_non_positional_data_stride() };
			UINT offsets[] = { m->get_positional_data_offset(), m->get_non_positional_data_offset() };

			ID3D11Buffer * buffers[] = { m->get_positional_data(), m->get_non_positional_data() };

			context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
			context->IASetIndexBuffer(m->get_indices_data(), m->get_indices_format(), m->get_indices_offset());
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			context->DrawIndexed(m->get_num_indices(), 0, 0);
		}
	}

	program.dispose(context);
}

void forward_renderer::update_render_variables(const render_variables & rvars, const render_variables_updates & update)
{
	m_rvars = rvars;
}