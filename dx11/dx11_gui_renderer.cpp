#include <xng/dx11/dx11_gui_renderer.hpp>
#include <xng/geometry.hpp>

using namespace xng::gui;
using namespace xng::dx11;
using namespace xng::math;

struct __vertex
{
	float2 position;
	float2 uv;
	float4 color;
};

struct alignas(16) __cbPerFrame
{
	float4x4 projectionMatrix;
};

bool dx11_gui_renderer::init(dx11_api_context * context)
{
	m_device = context->get_device();
	set_device_context(context->get_immediate_context());

	return m_program.preprocess(XNG_DX11_SHADER_FILE("gui.xhlsl")) &&
		m_constantBuffer.create<__cbPerFrame>(m_device.get()) &&
		!XNG_HR_FAILED(context->get_device()->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(__vertex) * 4, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
			nullptr,
			m_vertexBuffer.reset_and_get_address()));
}

void dx11_gui_renderer::shutdown(void)
{
	m_program.clear();
	m_constantBuffer.reset();
	m_device.reset();
	m_deviceContext.reset();
	m_vertexBuffer.reset();
}

void dx11_gui_renderer::set_device_context(ID3D11DeviceContext * deviceContext)
{
	m_deviceContext = deviceContext;
}

void dx11_gui_renderer::render_begin(const uint2 & size)
{
	UINT strides = sizeof(__vertex);
	UINT offsets = 0;

	m_deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.get_address(), &strides, &offsets);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	__cbPerFrame data = {
		ortho_rh_directx(0, size.x, size.y, 0, 0, 1)
	};

	m_constantBuffer.write(m_deviceContext.get(), &data);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void dx11_gui_renderer::render_end(void)
{

}

void dx11_gui_renderer::render_filled_rectangle(const rectangle & rect, const float4 & color)
{
	shader_program shader = m_program.compile(m_device.get(), "filled_rectangle");

	shader.use(m_deviceContext.get());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	if (!XNG_HR_FAILED(m_deviceContext->Map(m_vertexBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		__vertex * vertices = (__vertex*)mappedResource.pData;

		vertices[0].position.x = rect.left;
		vertices[0].position.y = rect.top;
		vertices[0].uv         = float2(0);
		vertices[0].color      = color;

		vertices[1].position.x = rect.left;
		vertices[1].position.y = rect.bottom;
		vertices[1].uv         = float2(0);
		vertices[1].color      = color;

		vertices[2].position.x = rect.right;
		vertices[2].position.y = rect.top;
		vertices[2].uv         = float2(0);
		vertices[2].color      = color;
		
		vertices[3].position.x = rect.right;
		vertices[3].position.y = rect.bottom;
		vertices[3].uv         = float2(0);
		vertices[3].color      = color;

		m_deviceContext->Unmap(m_vertexBuffer.get(), 0);
		m_deviceContext->Draw(4, 0);
	}

	shader.dispose(m_deviceContext.get());
}