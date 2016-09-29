#pragma once

#include <xng/dx11/gpu_mesh.hpp>

#include <xng/graphics/mesh.hpp>
#include <xng/res.hpp>

#include <string>

using namespace xng::dx11;
using namespace xng::res;

const char * gpu_mesh::resource_type = "dx11mesh";

gpu_mesh::gpu_mesh(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	resource(name, params, loader, owner) {}

gpu_mesh::~gpu_mesh(void)
{
	clear();
}

void gpu_mesh::clear(void)
{
	m_position.reset();
	m_nonPositionalData.reset();
	m_indices.reset();

	m_positionStride          = 0;
	m_positionOffset          = 0;

	m_nonPositionalDataStride = 0;
	m_nonPositionalDataOffset = 0;

	m_indicesFormat           = DXGI_FORMAT_UNKNOWN;
	m_indicesOffset           = 0;

	m_numVertices             = 0;
	m_numIndices              = 0;
}

bool gpu_mesh::load_impl(const void * userdata)
{
	graphics::mesh_ptr m(dynamic_cast<graphics::mesh*>(get_dependency()));

	if (!m)
	{
		m = resource_factory::get_singleton()->create<graphics::mesh>(get_name());
	}

	if (m && m->load())
	{
		bool success;

		ID3D11Device * device = static_cast<const load_data *>(userdata)->device;

		clear();

		m_numVertices = m->get_num_vertices();
		m_numIndices  = m->get_num_indices();

		{
			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = m->get_vertices();

			success = !XNG_HR_FAILED(device->CreateBuffer(
				&CD3D11_BUFFER_DESC(3 * sizeof(float) * m->get_num_vertices(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&m_position
			));

			m_positionStride = sizeof(float) * 3;
			m_positionOffset = 0;
		}

		{
			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = m->get_indices();

			success = success && !XNG_HR_FAILED(device->CreateBuffer(
				&CD3D11_BUFFER_DESC(sizeof(uint32_t) * m->get_num_indices(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&m_indices
			));
			
			m_indicesFormat = DXGI_FORMAT_R32_UINT;
			m_indicesOffset = 0;
		}

		if (m->get_storage_semantic_flags() != XNG_MESH_STORAGE_NONE)
		{
			std::vector<uint8_t> nonPositionalData = m->pack_interleaved_data(false, m->get_storage_semantic_flags());

			D3D11_SUBRESOURCE_DATA initializer = {};

			initializer.pSysMem = nonPositionalData.data();

			success = success && !XNG_HR_FAILED(device->CreateBuffer(
				&CD3D11_BUFFER_DESC(nonPositionalData.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE),
				&initializer,
				&m_nonPositionalData
			));

			m_nonPositionalDataStride = m->calculate_stride(m->get_storage_semantic_flags()) - sizeof(float) * 3;
			m_nonPositionalDataOffset = 0;
		}

		return success;
	}

	return false;
}

void gpu_mesh::unload_impl(void)
{
	clear();
}

size_t gpu_mesh::calculate_size_impl(void)
{
	return m_numVertices * (m_positionStride + m_nonPositionalDataStride) + m_numIndices * sizeof(uint32_t);
}