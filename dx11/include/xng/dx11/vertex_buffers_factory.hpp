#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/graphics.hpp>

#include <unordered_map>

namespace xng
{
	namespace dx11
	{
		class vertex_buffers_factory
		{

		public:

			struct buffers
			{
				com_ptr<ID3D11Buffer> positions;
				com_ptr<ID3D11Buffer> nonPositionalData;
				com_ptr<ID3D11Buffer> indices;

				UINT strides[2];
				UINT offsets[2];
			};

			buffers create(ID3D11Device * device, graphics::mesh_ptr m);
			void clear(void);

		private:

			std::unordered_map<res::resource_id, buffers> m_buffers;

		};
	}
}