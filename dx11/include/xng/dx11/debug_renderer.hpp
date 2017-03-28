#pragma once

#include <xng/dx11.hpp>
#include <xng/graphics.hpp>
#include <xng/geometry.hpp>
#include <xng/dx11/uber_shader.hpp>

#include <vector>

namespace xng
{
	namespace dx11
	{
		class debug_renderer
		{

		public:

			debug_renderer(void) = default;
			debug_renderer(const debug_renderer &) = delete;
			debug_renderer(debug_renderer &&) = default;

			bool init(dx11_api_context * context, const graphics::render_variables & rvars);
			void shutdown(void);

			void render_begin(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const graphics::camera * camera);
			void render_end(void);

			void render(const math::aabb & bb, const math::float4 & color);
			void render(const math::frustum & f, const math::float4 & color);
			void render(const math::float3 & x0, const math::float3 & x1, const math::float4 & color);

		private:

			struct vertex
			{
				math::float3 position;
				math::float4 color;
			};

			std::vector<vertex>   m_lines;
			com_ptr<ID3D11Buffer> m_vb;
			constant_buffer       m_cbPerFrame;
			size_t                m_vbSize;

			dx11_api_context * m_apiContext;

			com_ptr<ID3D11DeviceContext>    m_context;
			com_ptr<ID3D11RenderTargetView> m_rtv;
			com_ptr<ID3D11DepthStencilView> m_dsv;

			uber_shader m_program;

		};
	}
}