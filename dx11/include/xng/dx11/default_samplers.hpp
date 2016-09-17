#pragma once

#include <xng/core.hpp>
#include <xng/dx11/dx11_headers.hpp>

#include <array>

namespace xng
{
	namespace dx11
	{
		class default_samplers
		{

		public:

			default_samplers(void);
			~default_samplers(void);

			bool init(ID3D11Device * device);
			void shutdown(void);

			void bind_vertex_shader(ID3D11DeviceContext * context);
			void bind_pixel_shader(ID3D11DeviceContext * context);
			void bind_compute_shader(ID3D11DeviceContext * context);

		private:

			std::array<com_ptr<ID3D11SamplerState>, 6> m_samplers;
			uint32_t m_anisotropy;

		};
	}
}