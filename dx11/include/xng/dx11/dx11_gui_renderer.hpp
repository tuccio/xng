#pragma once

#include <xng/gui/gui_renderer.hpp>
#include <xng/dx11/dx11_headers.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>

namespace xng
{
	namespace dx11
	{
		class dx11_gui_renderer :
			public gui::gui_renderer
		{

		public:

			bool init(dx11_api_context * context);
			void shutdown(void);

			void set_device_context(ID3D11DeviceContext * deviceContext);

			void render_begin(const math::uint2 & size) override;
			void render_end(void) override;

			void render_filled_rectangle(const gui::rectangle & rect, const math::float4 & color) override;

		private:

			com_ptr<ID3D11Device>        m_device;

			com_ptr<ID3D11DeviceContext> m_deviceContext;
			com_ptr<ID3D11Buffer>        m_vertexBuffer;

			constant_buffer              m_constantBuffer;

			uber_shader                  m_program;

		};
	}
}