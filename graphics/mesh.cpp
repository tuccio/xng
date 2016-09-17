#include <xng/graphics/mesh.hpp>

using namespace xng::res;
using namespace xng::math;
using namespace xng::graphics;

const char * mesh::resource_type = "mesh";

mesh::mesh(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	resource(name, params, loader, owner) {}

mesh::~mesh(void)
{
	clear();
}

bool mesh::create(size_t vertices, size_t triangles, uint32_t storage)
{
	clear();

	m_numVertices  = vertices;
	m_numTriangles = triangles;

	m_storageFlags = storage;

	m_indices.resize(m_numTriangles);
	m_vertices.resize(m_numVertices);

	if (has_storage_semantic(XNG_MESH_STORAGE_NORMALS))
	{
		m_normals.resize(m_numVertices);
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_TANGENTS))
	{
		m_tangents.resize(m_numVertices);
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_BITANGENTS))
	{
		m_bitangents.resize(m_numVertices);
	}

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		uint32_t texcoordSemantic = XNG_MESH_STORAGE_TEXCOORDS0 << i;

		if (has_storage_semantic(static_cast<mesh_storage_semantic>(texcoordSemantic)))
		{
			m_texcoords[i].resize(m_numVertices);
		}
	}

	return true;
}

uint32_t mesh::get_parameters_storage_semantic_flags(void)
{
	auto & params = get_parameters();

	boost::optional<bool> normalsOpt    = params.get_optional<bool>("normals");
	boost::optional<bool> tangentsOpt   = params.get_optional<bool>("tangents");
	boost::optional<bool> bitangentsOpt = params.get_optional<bool>("bitangents");
	boost::optional<bool> texcoords0Opt = params.get_optional<bool>("texcoords0");
	boost::optional<bool> texcoords1Opt = params.get_optional<bool>("texcoords1");

	uint32_t flags = 0;

	/* Default */

	if (!normalsOpt || *normalsOpt) flags |= XNG_MESH_STORAGE_NORMALS;
	if (!tangentsOpt || *tangentsOpt) flags |= XNG_MESH_STORAGE_TANGENTS;
	if (!bitangentsOpt || *bitangentsOpt) flags |= XNG_MESH_STORAGE_TANGENTS;
	if (!texcoords0Opt || *texcoords0Opt) flags |= XNG_MESH_STORAGE_TEXCOORDS0;

	/* Non default */

	if (texcoords1Opt && *texcoords1Opt) flags |= XNG_MESH_STORAGE_TEXCOORDS1;

	return flags;
}

bool mesh::calculate_tangent_space(void)
{
	if (!has_storage_semantic(XNG_MESH_STORAGE_NORMALS))
	{
		return false;
	}

	add_storage_semantic(XNG_MESH_STORAGE_TANGENTS);
	add_storage_semantic(XNG_MESH_STORAGE_BITANGENTS);

	//if (!has_storage_semantic(XNG_MESH_STORAGE_TEXCOORDS0))
	{
		for (int i = 0; i < m_numVertices; i++)
		{
			float absNx = fabs(m_normals[i].x);

			float3 N, T, B;

			N = m_normals[i];
			T = float3{ absNx > .99f ? 0.f : 1.f, absNx > .99f ? 1.f : 0.f, 0.f };
			B = cross(N, T);
			T = cross(N, B);

			m_tangents[i]   = T;
			m_bitangents[i] = B;
		}
	}
	//else
	{
		//throw;
		// Tangent space with texcoord
	}

	return true;
}

void mesh::clear(void)
{
	m_vertices.clear();
	m_vertices.shrink_to_fit();

	m_normals.clear();
	m_normals.shrink_to_fit();

	m_tangents.clear();
	m_tangents.shrink_to_fit();

	m_bitangents.clear();
	m_bitangents.shrink_to_fit();

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; ++i)
	{
		m_texcoords[i].clear();
		m_texcoords[i].shrink_to_fit();
	}

	m_numVertices  = m_numTriangles = 0;
	m_storageFlags = 0;
}

bool mesh::load_impl(const void * userdata)
{
	// TODO: Implement own mesh file format loader
	return false;
}

void mesh::unload_impl(void)
{
}

size_t mesh::calculate_size_impl(void)
{
	return calculate_size_internal();
}

size_t mesh::calculate_size_internal(void) const
{
	size_t indicesSize   = m_numTriangles * 3 * sizeof(uint32_t);
	size_t verticesSize  = m_numVertices * 3 * sizeof(float);
	size_t texcoordsSize = m_numVertices * 2 * sizeof(float);

	int texcoordsMultiplier = 1;
	int verticesMultiplier  = 1;

	if (has_storage_semantic(XNG_MESH_STORAGE_NORMALS))
	{
		++verticesMultiplier;
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_TANGENTS))
	{
		++verticesMultiplier;
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_BITANGENTS))
	{
		++verticesMultiplier;
	}

	uint32_t texcoordFlag = XNG_MESH_STORAGE_TEXCOORDS0;

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		if (m_storageFlags & texcoordFlag)
		{
			++texcoordsMultiplier;
		}

		texcoordFlag <<= 1;
	}

	return verticesSize * verticesMultiplier + texcoordsSize * texcoordsMultiplier + indicesSize;
}

bool mesh::add_storage_semantic(mesh_storage_semantic semantic)
{
	if (has_storage_semantic(semantic))
	{
		return false;
	}

	m_storageFlags |= semantic;

	switch (semantic)
	{
	case XNG_MESH_STORAGE_NORMALS:
		{
			m_normals.resize(m_numVertices);
			return true;
		}

	case XNG_MESH_STORAGE_TANGENTS:
		{
			m_tangents.resize(m_numVertices);
			return true;
		}

	case XNG_MESH_STORAGE_BITANGENTS:
		{
			m_bitangents.resize(m_numVertices);
			return true;
		}		
	}

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		uint32_t texcoordSemantic = XNG_MESH_STORAGE_TEXCOORDS0 << i;

		if (semantic == texcoordSemantic)
		{
			m_texcoords[i].resize(m_numVertices);
			return true;
		}
	}

	return false;
}

bool mesh::remove_storage_semantic(mesh_storage_semantic semantic)
{
	if (!has_storage_semantic(semantic))
	{
		return false;
	}

	m_storageFlags &= ~semantic;

	switch (semantic)
	{
	case XNG_MESH_STORAGE_NORMALS:
	{
		m_normals.clear();
		m_normals.shrink_to_fit();
		return true;
	}

	case XNG_MESH_STORAGE_TANGENTS:
	{
		m_tangents.clear();
		m_tangents.shrink_to_fit();
		return true;
	}

	case XNG_MESH_STORAGE_BITANGENTS:
	{
		m_bitangents.clear();
		m_bitangents.shrink_to_fit();
		return true;
	}
	}

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		uint32_t texcoordSemantic = XNG_MESH_STORAGE_TEXCOORDS0 << i;

		if (semantic == texcoordSemantic)
		{
			m_texcoords[i].clear();
			m_texcoords[i].shrink_to_fit();
			return true;
		}
	}

	return false;
}

std::vector<uint8_t> mesh::pack_interleaved_data(bool packPositions, uint32_t flags) const
{
	size_t bufferSize = calculate_stride(flags) * get_num_vertices();

	std::vector<uint8_t> buffer(bufferSize);
	size_t offset = 0;

	uint8_t * bufferPosition = &buffer[0];
	std::vector<const uint8_t*> iterators;
	std::vector<uint32_t> dataSize;

	// Setup all the iterators

	if (packPositions)
	{
		iterators.push_back(reinterpret_cast<const uint8_t*>(get_vertices()));
		dataSize.push_back(3 * sizeof(float));
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_NORMALS))
	{
		iterators.push_back(reinterpret_cast<const uint8_t*>(get_normals()));
		dataSize.push_back(3 * sizeof(float));
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_TANGENTS))
	{
		iterators.push_back(reinterpret_cast<const uint8_t*>(get_tangents()));
		dataSize.push_back(3 * sizeof(float));
	}

	if (has_storage_semantic(XNG_MESH_STORAGE_BITANGENTS))
	{
		iterators.push_back(reinterpret_cast<const uint8_t*>(get_bitangents()));
		dataSize.push_back(3 * sizeof(float));
	}

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		uint32_t texcoordSemantic = XNG_MESH_STORAGE_TEXCOORDS0 << i;

		if (has_storage_semantic(static_cast<mesh_storage_semantic>(texcoordSemantic)))
		{
			iterators.push_back(reinterpret_cast<const uint8_t*>(get_texcoords(i)));
			dataSize.push_back(2 * sizeof(float));
		}
	}

	// Copy using the iterators

	for (int vertexIndex = 0; vertexIndex < m_numVertices; vertexIndex++)
	{
		for (int iteratorIndex = 0; iteratorIndex < iterators.size(); iteratorIndex++)
		{
			uint32_t copySize = dataSize[iteratorIndex];

			std::memcpy(bufferPosition, iterators[iteratorIndex], copySize);
			bufferPosition += copySize;

			offset += copySize;
			iterators[iteratorIndex] += copySize;
		}
	}

	return buffer;
}

size_t mesh::calculate_stride(uint32_t flags)
{
	uint32_t texcoordsMultiplier = 0;
	uint32_t verticesMultiplier  = 1;

	if (flags & XNG_MESH_STORAGE_NORMALS)
	{
		++verticesMultiplier;
	}

	if (flags & XNG_MESH_STORAGE_TANGENTS)
	{
		++verticesMultiplier;
	}

	if (flags & XNG_MESH_STORAGE_BITANGENTS)
	{
		++verticesMultiplier;
	}

	uint32_t texcoordFlag = XNG_MESH_STORAGE_TEXCOORDS0;

	for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
	{
		if (flags & texcoordFlag)
		{
			++texcoordsMultiplier;
		}

		texcoordFlag <<= 1;
	}

	return sizeof(float) * 3 * verticesMultiplier + sizeof(float) * 2 * texcoordsMultiplier;
}

// box_mesh_loader

bool box_mesh_loader::load(resource * r, const void * userdata)
{
	mesh * m = static_cast<mesh*>(r);

	uint32_t flags = m->get_parameters_storage_semantic_flags();

	if (flags == XNG_MESH_STORAGE_NONE)
	{
		// Minimal version of the mesh with no texcoords or normals (might be useful for occlusion culling)

		if (m->create(8, 12, flags))
		{
			auto & params = m->get_parameters();

			auto pCenter = params.get_optional<float3>("center");
			auto pExtents = params.get_optional<float3>("extents");

			float3 center = pCenter ? *pCenter : float3(0);
			float3 halfExtents = .5f * (pExtents ? *pExtents : float3(1));

			float3 vertices[8] = {
				center - halfExtents,
				center + float3(-halfExtents.x, -halfExtents.y, halfExtents.z),
				center + float3(halfExtents.x, -halfExtents.y, halfExtents.z),
				center + float3(halfExtents.x, -halfExtents.y, -halfExtents.z),
				center + float3(-halfExtents.x, halfExtents.y, -halfExtents.z),
				center + float3(-halfExtents.x, halfExtents.y, halfExtents.z),
				center + float3(halfExtents.x, halfExtents.y, halfExtents.z),
				center + float3(halfExtents.x, halfExtents.y, -halfExtents.z)
			};

			uint3 indices[12] = {
				// Bottom quad
				{ 0, 2, 1 },
				{ 0, 3, 2 },

				// Top quad
				{ 4, 5, 6 },
				{ 4, 6, 7 },

				// Left quad
				{ 3, 7, 6 },
				{ 3, 6, 2 },

				// Right quad
				{ 0, 1, 5 },
				{ 0, 5, 4 }
			};

			memcpy(m->get_vertices(), vertices, sizeof(vertices));
			memcpy(m->get_indices(), indices, sizeof(indices));

			return true;
		}
	}

	return false;
}

void box_mesh_loader::unload(resource * r)
{
	mesh * m = static_cast<mesh*>(r);
	m->clear();
}