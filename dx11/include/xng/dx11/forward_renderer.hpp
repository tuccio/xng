#pragma once

#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/dx11/vertex_buffers_factory.hpp>

namespace xng
{
	namespace dx11
	{
		class forward_renderer 
		{

		public:

			bool init(dx11_api_context * context);
			void shutdown(void);

			void update_render_variables(const graphics::render_variables & rvars, const graphics::render_variables_updates & update);
			void render(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const graphics::extracted_scene & extractedScene);

		private:

			dx11_api_context * m_apiContext;
			graphics::render_variables m_rvars;

			com_ptr<ID3D11RasterizerState> m_rasterizerState;

			constant_buffer m_cbPerObject;

			vertex_buffers_factory m_vbFactory;
			uber_shader m_program;

		};
	}
}