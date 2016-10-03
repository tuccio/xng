#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/gui/gui_command_list.hpp>

namespace xng
{
	namespace dx11
	{
		class dx11_gui_renderer
		{

		public:

			bool init(dx11_api_context * context);
			void shutdown(void);

			void render(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, const math::uint2 & size, const gui::gui_command_list & commandList);

			void reload_shaders(void);

		private:

			void render_filled_rectangle(ID3D11DeviceContext * deviceContext, const gui::rectangle & rect, const math::float4 & color);

			void render_textured_rectangle(
				ID3D11DeviceContext * deviceContext,
				const gui::rectangle & rect,
				const math::float2 & uv0,
				const math::float2 & uv1,
				graphics::image_ptr texture);

			void render_text(
				ID3D11DeviceContext * deviceContext,
				graphics::font_ptr fnt,
				const wchar_t * text,
				const math::float4 & color,
				const math::float4 & borderColor, uint32_t borderSize,
				float width,
				const math::uint2 & position, const math::float2 & scale);

		private:

			com_ptr<ID3D11Device>          m_device;

			com_ptr<ID3D11RasterizerState> m_rasterizerState;
			com_ptr<ID3D11BlendState>      m_blendState;

			com_ptr<ID3D11Buffer>          m_quadBuffer;
			com_ptr<ID3D11Buffer>          m_textBuffer;

			constant_buffer                m_constantBuffer;

			uber_shader                    m_program;

			math::float4x4                 m_projection;

		};
	}
}