#include <xng/dx11/vertex_buffers_factory.hpp>

using namespace xng::graphics;
using namespace xng::dx11;
using namespace xng::math;

vertex_buffers_factory::buffers vertex_buffers_factory::create(ID3D11Device * device, mesh_ptr m)
{
	buffers b = {};

	auto it = m_buffers.find(m->get_id());

	if (it == m_buffers.end() && m->load())
	{
		{
			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = m->get_vertices();

			device->CreateBuffer(
				&CD3D11_BUFFER_DESC(3 * sizeof(float) * m->get_num_vertices(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&b.positions
			);

			b.strides[0] = 3 * sizeof(float);
		}

		{
			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = m->get_indices();

			device->CreateBuffer(
				&CD3D11_BUFFER_DESC(3 * sizeof(uint32_t) * m->get_num_indices(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&b.indices
			);
		}

		if (m->get_storage_semantic_flags() != XNG_MESH_STORAGE_NONE)
		{
			std::vector<uint8_t> nonPositionalData = m->pack_interleaved_data(false, m->get_storage_semantic_flags());
			
			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = nonPositionalData.data();

			device->CreateBuffer(
				&CD3D11_BUFFER_DESC(nonPositionalData.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&b.nonPositionalData
			);

			b.strides[1] = m->calculate_stride(m->get_storage_semantic_flags() - sizeof(float) * 3);
		}

		m_buffers[m->get_id()] = b;
	}
	else
	{
		b = it->second;
	}

	return b;
}

void vertex_buffers_factory::clear(void)
{
	m_buffers.clear();
}