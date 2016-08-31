#include <xng/dx11/forward_renderer.hpp>

#include <xng/dx11.hpp>
#include <xng/graphics/mesh.hpp>

using namespace xng::dx11;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

bool forward_renderer::init(api_context * context)
{
	m_apiContext = dynamic_cast<dx11_api_context*>(context);

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

void forward_renderer::render(scene * scene, const camera * camera, render_resource * target)
{
	render_variables rvars;
	process_rv_updates(&rvars);

	ID3D11Device        * device  = m_apiContext->get_device();
	ID3D11DeviceContext * context = m_apiContext->get_immediate_context();

	float clearColor[4] = { 0 };

	ID3D11RenderTargetView * backBuffer = m_apiContext->get_back_buffer_rtv();

	context->ClearRenderTargetView(backBuffer, clearColor);
	context->RSSetViewports(1, &CD3D11_VIEWPORT(0.f, 0.f, rvars.render_resolution.x, rvars.render_resolution.y));

	mesh_ptr triangle = resource_factory::get_singleton()->create<mesh>(
		"mesh", "triangle", resource_parameters(),
		std::make_shared<dynamic_resource_loader>(
			[](resource * r)
	{
		mesh * m = static_cast<mesh*>(r);

		if (m->create(3, 1, XNG_MESH_STORAGE_NONE))
		{
			float vertices[] = {
				-1.f, -1.f, 0.f,
				1.f, -1.f, 0.f,
				0.f,  1.f, 0.f,
			};

			uint32_t indices[] = {
				0, 1, 2
			};

			memcpy(m->get_vertices(), vertices, sizeof(vertices));
			memcpy(m->get_indices(), indices, sizeof(indices));

			return true;
		}

		return false;
	},
		[](resource * r)
	{
		static_cast<mesh*>(r)->clear();
	}
	));

	float4 colors[] = {
		{ 1, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 }
	};

	context->RSSetState(m_rasterizerState.get());
	context->OMSetRenderTargets(1, &backBuffer, nullptr);

	m_cbPerObject.write(context, &colors[0]);
	//auto b = m_vbFactory.create(device, triangle);

	//context->IASetVertexBuffers(0, 1, &b.positions, b.strides, b.offsets);
	//context->IASetIndexBuffer(b.indices.get(), DXGI_FORMAT_R32_UINT, 0);

	gpu_mesh_ptr m = make_gpu_mesh(triangle);

	if (m && m->load())
	{
		UINT strides[] = { m->get_positional_data_stride(), m->get_non_positional_data_stride() };
		UINT offsets[] = { m->get_positional_data_offset(), m->get_non_positional_data_offset() };

		ID3D11Buffer * buffers[] = { m->get_positional_data(), m->get_non_positional_data() };

		context->IASetVertexBuffers(0, 1, buffers, strides, offsets);
		context->IASetIndexBuffer(m->get_indices_data(), m->get_indices_format(), m->get_indices_offset());

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		shader_program program = m_program.compile(m_apiContext->get_device());

		program.use(context);

		context->DrawIndexed(m->get_num_indices(), 0, 0);

		program.dispose(context);
	}
}

void forward_renderer::process_rv_updates(render_variables * rvars)
{
	std::set<xng_render_variable> updates;
	get_render_variables(rvars, &updates);
}