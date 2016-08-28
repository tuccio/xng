#pragma once

#include <xng/graphics/renderer.hpp>
#include <xng/dx11/dx11_api_context.hpp>
#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/constant_buffer.hpp>
#include <xng/dx11/vertex_buffers_factory.hpp>

namespace xng
{
	namespace dx11
	{
		class forward_renderer :
			public graphics::renderer
		{

		public:

			bool init(graphics::api_context * context) override;
			void shutdown(void) override;

			void render(graphics::scene * scene, graphics::camera * camera, graphics::render_resource * target = nullptr) override;

		private:

			void process_rv_updates(graphics::render_variables * rvars);
			
			dx11_api_context * m_apiContext;

			com_ptr<ID3D11RasterizerState> m_rasterizerState;

			constant_buffer m_cbPerObject;

			vertex_buffers_factory m_vbFactory;
			uber_shader m_program;

		};
	}
}