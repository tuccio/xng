#include <xng/dx11/dx11_gui_renderer.hpp>
#include <xng/geometry.hpp>
#include <xng/dx11/texture.hpp>

using namespace xng::core;
using namespace xng::gui;
using namespace xng::dx11;
using namespace xng::math;
using namespace xng::res;
using namespace xng::graphics;

#define MAX_VERTICES 1024

struct __vertex
{
	float2 position;
	float2 uv;
};

struct alignas(16) __cbPerFrame
{
	float4x4 ProjectionMatrix;
	float4   Color;
	float4   BorderColor;
	float    BorderSize;
	float    SmoothnessFactor;
};

bool dx11_gui_renderer::init(dx11_api_context * context)
{
	m_device = context->get_device();
	set_device_context(context->get_immediate_context());

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

	blendDesc.RenderTarget[0].BlendEnable           = TRUE;
	blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return m_program.preprocess(XNG_DX11_SHADER_FILE("gui.xhlsl")) &&
		m_constantBuffer.create<__cbPerFrame>(m_device.get()) &&
		!XNG_HR_FAILED(m_device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(__vertex) * 4, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
			nullptr,
			m_quadBuffer.reset_and_get_address())) &&
		!XNG_HR_FAILED(m_device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(font::vertex) * MAX_VERTICES, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
			nullptr,
			m_textBuffer.reset_and_get_address())) &&
		!XNG_HR_FAILED(m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.reset_and_get_address())) &&
		!XNG_HR_FAILED(m_device->CreateBlendState(&blendDesc, m_blendState.reset_and_get_address()));
}

void dx11_gui_renderer::shutdown(void)
{
	m_program.clear();
	m_constantBuffer.reset();
	m_device.reset();
	m_deviceContext.reset();
	m_quadBuffer.reset();
	m_textBuffer.reset();
}

void dx11_gui_renderer::set_device_context(ID3D11DeviceContext * deviceContext)
{
	m_deviceContext = deviceContext;
}

void dx11_gui_renderer::render_begin(const uint2 & size)
{
	m_projection = ortho_rh_directx(0, size.x, size.y, 0, 0, 1);

	m_deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	m_deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	m_deviceContext->RSSetState(m_rasterizerState.get());
	m_deviceContext->OMSetBlendState(m_blendState.get(), nullptr, 0xFFFFFFFF);
}

void dx11_gui_renderer::render_end(void)
{

}

void dx11_gui_renderer::render_filled_rectangle(const rectangle & rect, const float4 & color)
{
	shader_program shader = m_program.compile(m_device.get(), "filled_rectangle");

	shader.use(m_deviceContext.get());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	if (!XNG_HR_FAILED(m_deviceContext->Map(m_quadBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		__cbPerFrame data = {
			m_projection, color
		};

		m_constantBuffer.write(m_deviceContext.get(), &data);

		__vertex * vertices = (__vertex*)mappedResource.pData;

		vertices[0].position.x = rect.left;
		vertices[0].position.y = rect.top;
		vertices[0].uv         = float2(0);

		vertices[1].position.x = rect.left;
		vertices[1].position.y = rect.bottom;
		vertices[1].uv         = float2(0);

		vertices[2].position.x = rect.right;
		vertices[2].position.y = rect.top;
		vertices[2].uv         = float2(0);
		
		vertices[3].position.x = rect.right;
		vertices[3].position.y = rect.bottom;
		vertices[3].uv         = float2(0);

		m_deviceContext->Unmap(m_quadBuffer.get(), 0);

		UINT strides = sizeof(__vertex);
		UINT offsets = 0;


		m_deviceContext->IASetVertexBuffers(0, 1, m_quadBuffer.get_address(), &strides, &offsets);
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		m_deviceContext->Draw(4, 0);
	}

	shader.dispose(m_deviceContext.get());
}

void dx11_gui_renderer::render_textured_rectangle(
	const rectangle & rect,
	const float2 & uv0,
	const float2 & uv1,
	image_ptr img)
{
	texture_ptr tex = resource_factory::get_singleton()->create<texture>("", resource_parameters(), resource_loader_ptr(), img);

	if (tex && tex->load(&texture::load_data(m_device.get(), m_deviceContext.get())))
	{
		shader_program shader = m_program.compile(m_device.get(), "textured_rectangle",
		{ { "XNG_TEXTURED", "" },{ nullptr, nullptr } });

		shader.use(m_deviceContext.get());

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (tex->load(&texture::load_data(m_device.get(), m_deviceContext.get())) &&
			!XNG_HR_FAILED(m_deviceContext->Map(m_quadBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			__cbPerFrame data = {
				m_projection
			};

			m_constantBuffer.write(m_deviceContext.get(), &data);

			__vertex * vertices = (__vertex*)mappedResource.pData;

			vertices[0].position.x = rect.left;
			vertices[0].position.y = rect.top;
			vertices[0].uv = uv0;

			vertices[1].position.x = rect.left;
			vertices[1].position.y = rect.bottom;
			vertices[1].uv = float2(uv0.x, uv1.y);

			vertices[2].position.x = rect.right;
			vertices[2].position.y = rect.top;
			vertices[2].uv = float2(uv1.x, uv0.y);

			vertices[3].position.x = rect.right;
			vertices[3].position.y = rect.bottom;
			vertices[3].uv = uv1;

			ID3D11ShaderResourceView * srv = tex->get_shader_resource_view();

			m_deviceContext->PSSetShaderResources(0, 1, &srv);

			m_deviceContext->Unmap(m_quadBuffer.get(), 0);

			UINT strides = sizeof(__vertex);
			UINT offsets = 0;

			m_deviceContext->IASetVertexBuffers(0, 1, m_quadBuffer.get_address(), &strides, &offsets);
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			m_deviceContext->Draw(4, 0);
		}

		shader.dispose(m_deviceContext.get());
	}
}

void dx11_gui_renderer::render_text(font_ptr fnt, const wchar_t * text, const float4 & color, const float4 & borderColor, uint32_t borderSize, float smoothness, const uint2 & position, const float2 & scale)
{
	if (fnt && fnt->load())
	{
		texture_ptr tex = resource_factory::get_singleton()->create<texture>("", fnt->get_parameters(), resource_loader_ptr(), fnt->get_image());

		if (tex && tex->load(&texture::load_data(m_device.get(), m_deviceContext.get())))
		{			
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			if (!XNG_HR_FAILED(m_deviceContext->Map(m_textBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			{
				__cbPerFrame data = {
					m_projection * to_translation4(float3(position.x, position.y, 0)) * to_scale4(float3(scale, 1.f)),
					color, borderColor, borderSize, smoothness
				};
				
				m_constantBuffer.write(m_deviceContext.get(), &data);

				shader_program shader = m_program.compile(m_device.get(), "text",
					{ { "XNG_TEXT", "" }, { nullptr, nullptr } });

				shader.use(m_deviceContext.get());
				
				ID3D11ShaderResourceView * srv = tex->get_shader_resource_view();

				m_deviceContext->PSSetShaderResources(1, 1, &srv);

				font::text_info info;

				font::vertex * vertices = (font::vertex*)mappedResource.pData;
				fnt->make_text(text, vertices, &info, int2(borderSize));

				UINT strides = sizeof(font::vertex);
				UINT offsets = 0;

				m_deviceContext->IASetVertexBuffers(0, 1, m_textBuffer.get_address(), &strides, &offsets);
				m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				m_deviceContext->Unmap(m_textBuffer.get(), 0);
				m_deviceContext->Draw(info.numVertices, 0);

				shader.dispose(m_deviceContext.get());
			}
			
		}
	}
	
}

void dx11_gui_renderer::render(ID3D11DeviceContext * deviceContext, const uint2 & size, const gui_command_list & commandList)
{
	set_device_context(deviceContext);

	render_begin(size);

	for (auto & cmd : commandList)
	{
		switch (cmd.type)
		{
		case XNG_GUI_COMMAND_FILLED_RECTANGLE:
			render_filled_rectangle(cmd.rect, cmd.color);
			break;

		case XNG_GUI_COMMAND_TEXTURED_RECTANGLE:
			render_textured_rectangle(cmd.rect, cmd.uv0, cmd.uv1, cmd.image);
			break;

		case XNG_GUI_COMMAND_TEXT:
			render_text(cmd.font, cmd.text.c_str(), cmd.color, cmd.border_color, cmd.border_size, cmd.smoothness, cmd.position, cmd.scale);
			break;
		}

	}

	render_end();
}