#include <xng/dx11/dx11_gui_renderer.hpp>
#include <xng/geometry.hpp>
#include <xng/dx11/texture.hpp>

#include <xng/graphics/make_text.hpp>

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

struct alignas(16) __cbPerText
{
	float4x4 ProjectionMatrix;
	float4   Color;
	float4   BorderColor;
	float    BorderSize;
	float    FontThinness;
	float    FontSpreadFactor;
	float    FontScale;
	float2   FontTexelSize;
};

bool dx11_gui_renderer::init(dx11_api_context * context)
{
	m_device = context->get_device();

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
		m_constantBuffer.create<__cbPerText>(m_device.get()) &&
		!XNG_HR_FAILED(m_device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(__vertex) * 4, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
			nullptr,
			m_quadBuffer.reset_and_get_address())) &&
		!XNG_HR_FAILED(m_device->CreateBuffer(
			&CD3D11_BUFFER_DESC(sizeof(text_vertex) * MAX_VERTICES, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
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
	m_quadBuffer.reset();
	m_textBuffer.reset();
}

void dx11_gui_renderer::render_filled_rectangle(ID3D11DeviceContext * deviceContext, const rectangle & rect, const float4 & color)
{
	shader_program shader = m_program.compile(m_device.get(), "filled_rectangle");

	shader.use(deviceContext);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	if (!XNG_HR_FAILED(deviceContext->Map(m_quadBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		__cbPerText data = {
			m_projection, color
		};

		m_constantBuffer.write(deviceContext, &data);

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

		deviceContext->Unmap(m_quadBuffer.get(), 0);

		UINT strides = sizeof(__vertex);
		UINT offsets = 0;


		deviceContext->IASetVertexBuffers(0, 1, m_quadBuffer.get_address(), &strides, &offsets);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		deviceContext->Draw(4, 0);
	}

	shader.dispose(deviceContext);
}

void dx11_gui_renderer::render_textured_rectangle(
	ID3D11DeviceContext * deviceContext,
	const rectangle & rect,
	const float2 & uv0,
	const float2 & uv1,
	image_ptr img)
{
	texture_ptr tex = resource_factory::get_singleton()->create<texture>("", resource_parameters(), resource_loader_ptr(), img);

	if (tex && tex->load(&texture::load_data(m_device.get(), deviceContext)))
	{
		shader_program shader = m_program.compile(m_device.get(), "textured_rectangle",
		{ { "XNG_TEXTURED", "" },{ nullptr, nullptr } });

		shader.use(deviceContext);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (tex->load(&texture::load_data(m_device.get(), deviceContext)) &&
			!XNG_HR_FAILED(deviceContext->Map(m_quadBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			__cbPerText data = {
				m_projection
			};

			m_constantBuffer.write(deviceContext, &data);

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

			deviceContext->PSSetShaderResources(0, 1, &srv);

			deviceContext->Unmap(m_quadBuffer.get(), 0);

			UINT strides = sizeof(__vertex);
			UINT offsets = 0;

			deviceContext->IASetVertexBuffers(0, 1, m_quadBuffer.get_address(), &strides, &offsets);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			deviceContext->Draw(4, 0);
		}
		
		shader.dispose(deviceContext);
	}
}

void dx11_gui_renderer::render_text(ID3D11DeviceContext * deviceContext, font_ptr fnt, const wchar_t * text, const float4 & color, const float4 & borderColor, uint32_t borderSize, float width, const uint2 & position, const float2 & scale)
{
	const bool SuperSample = true;
	
	if (fnt && fnt->load())
	{
		texture_ptr tex = resource_factory::get_singleton()->create<texture>("", make_resource_parameters(), resource_loader_ptr(), fnt->get_image());

		if (tex && tex->load(&texture::load_data(m_device.get(), deviceContext)))
		{			
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			if (!XNG_HR_FAILED(deviceContext->Map(m_textBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			{
				float avgScale = (scale.x + scale.y) * .5f;
				float2 fontTexelSize = { 1.f / fnt->get_image()->get_width(), 1.f / fnt->get_image()->get_height() };
				float borderDistance = .5f * borderSize / fnt->get_spread_factor();


				__cbPerText data = {
					m_projection * to_translation4(float3(position.x, position.y, 0)) * to_scale4(float3(scale, 1.f)),
					color, borderColor, borderDistance, width, fnt->get_spread_factor(), avgScale, fontTexelSize
				};
				
				m_constantBuffer.write(deviceContext, &data);

				shader_program shader = m_program.compile(m_device.get(), "text",
					{ { SuperSample ? "XNG_TEXT_SUPERSAMPLE" : "XNG_TEXT", "" }, { nullptr, nullptr } });
				
				shader.use(deviceContext);
				
				ID3D11ShaderResourceView * srv = tex->get_shader_resource_view();

				deviceContext->PSSetShaderResources(1, 1, &srv);

				text_info info;

				text_vertex * vertices = (text_vertex*)mappedResource.pData;

				int2 spacing = borderSize + 2;
				make_text(fnt, text, vertices, int2(borderSize), spacing, &info);

				UINT strides = sizeof(text_vertex);
				UINT offsets = 0;

				deviceContext->IASetVertexBuffers(0, 1, m_textBuffer.get_address(), &strides, &offsets);
				deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				deviceContext->Unmap(m_textBuffer.get(), 0);
				deviceContext->Draw(info.numVertices, 0);

				shader.dispose(deviceContext);
			}
		}
	}
}

void dx11_gui_renderer::render(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, const uint2 & size, const gui_command_list & commandList)
{
	m_projection = ortho_rh_directx(0, size.x, size.y, 0, 0, 1);

	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	deviceContext->RSSetState(m_rasterizerState.get());
	deviceContext->OMSetBlendState(m_blendState.get(), nullptr, 0xFFFFFFFF);

	deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

	for (auto & cmd : commandList)
	{
		switch (cmd.type)
		{
		case XNG_GUI_COMMAND_FILLED_RECTANGLE:
			render_filled_rectangle(deviceContext, cmd.rect, cmd.color);
			break;

		case XNG_GUI_COMMAND_TEXTURED_RECTANGLE:
			render_textured_rectangle(deviceContext, cmd.rect, cmd.uv0, cmd.uv1, cmd.image);
			break;

		case XNG_GUI_COMMAND_TEXT:
			render_text(deviceContext, cmd.font, cmd.text.c_str(), cmd.color, cmd.border_color, cmd.border_size, cmd.thinness, cmd.position, cmd.scale);
			break;
		}

	}
}