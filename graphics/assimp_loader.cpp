#ifdef XNG_ASSIMP

#include <xng/graphics/assimp_loader.hpp>
#include <xng/core.hpp>

#include <xng/res/resource_factory.hpp>
#include <xng/res/resource_manager.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <algorithm>
#include <cstring>
#include <sstream>

using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;
using namespace xng::math;

assimp_loader::assimp_loader(const path & filename, unsigned int flags) :
	m_filename(filename)
{
	auto fnString = filename.string();

	m_scene = m_importer.ReadFile(fnString.c_str(), flags | aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!m_scene)
	{
		XNG_LOG("assimp_loader", XNG_LOG_STREAM() << "Failed to load scene \"" << fnString.c_str() << "\": " << m_importer.GetErrorString());
	}
}

assimp_loader::~assimp_loader(void)
{
	m_importer.FreeScene();
}

bool assimp_loader::load(resource * r, const void * userdata)
{
	const char * resourceType = r->get_owner()->get_type();

	if (auto rMesh = dynamic_cast<mesh*>(r))
	{
		return load_mesh(rMesh);
	}
	else if (auto rMaterial = dynamic_cast<material*>(r))
	{
		return load_material(rMaterial);
	}

	return false;
}

void assimp_loader::unload(resource * r)
{
	const char * resourceType = r->get_owner()->get_type();

	if (auto rMesh = dynamic_cast<mesh*>(r))
	{
		unload_mesh(rMesh);
	}
	else if (auto rMaterial = dynamic_cast<material*>(r))
	{
		unload_material(rMaterial);
	}
}

const aiScene * assimp_loader::get_scene(void) const
{
	return m_scene;
}

mesh_ptr assimp_loader::create_mesh(unsigned int meshIndex)
{
	std::string name = std::to_string(meshIndex) + "_assimp_mesh_" + m_filename.string();
	return resource_factory::get_singleton()->create<mesh>(name.c_str(), default_parameters(), this);
}

material_ptr assimp_loader::create_material(unsigned int materialIndex)
{
	std::string name = std::to_string(materialIndex) + "_assimp_material_" + m_filename.string();
	return resource_factory::get_singleton()->create<material>(name.c_str(), default_parameters(), this);
}

bool assimp_loader::load_mesh(mesh * m)
{
	const char * name = m->get_name();

	unsigned int id;
	std::stringstream ss(name);

	ss >> id;

	// Look for the resource in the loaded scene

	if (ss.fail() || id >= m_scene->mNumMeshes)
	{
		id = -1;
		for (int i = 0; i < m_scene->mNumMeshes; i++)
		{
			if (strcmp(name, m_scene->mMeshes[i]->mName.C_Str()) == 0)
			{
				id = i;
				break;
			}
		}
	}

	if (id >= 0)
	{
		// Load the mesh
		aiMesh * pMesh = m_scene->mMeshes[id];

		unsigned int flags = 0;

		if (pMesh->HasNormals()) flags |= XNG_MESH_STORAGE_NORMALS;
		if (pMesh->HasTangentsAndBitangents()) flags |= XNG_MESH_STORAGE_TANGENTSPACE;

		for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
		{
			if (pMesh->HasTextureCoords(i)) flags |= (XNG_MESH_STORAGE_TEXCOORDS0 << i);
		}

		flags &= m->get_parameters_storage_semantic_flags();

		m->create(pMesh->mNumVertices, pMesh->mNumFaces, flags);

		std::transform(pMesh->mFaces,
			pMesh->mFaces + pMesh->mNumFaces,
			(uint3*)m->get_indices(),
			[](const aiFace & face) { return uint3{ face.mIndices[0], face.mIndices[1], face.mIndices[2] }; });

		memcpy(m->get_vertices(), pMesh->mVertices, pMesh->mNumVertices * sizeof(float) * 3);

		if (m->has_storage_semantic(XNG_MESH_STORAGE_NORMALS) && pMesh->HasNormals())
		{
			memcpy(m->get_normals(), pMesh->mNormals, pMesh->mNumVertices * sizeof(float) * 3);
		}

		if (m->has_storage_semantic(XNG_MESH_STORAGE_TANGENTS) && pMesh->HasTangentsAndBitangents())
		{
			memcpy(m->get_tangents(), pMesh->mTangents, pMesh->mNumVertices * sizeof(float) * 3);
		}

		if (m->has_storage_semantic(XNG_MESH_STORAGE_BITANGENTS) && pMesh->HasTangentsAndBitangents())
		{
			memcpy(m->get_bitangents(), pMesh->mBitangents, pMesh->mNumVertices * sizeof(float) * 3);
		}

		for (int i = 0; i < XNG_MESH_MAX_TEXCOORDS; i++)
		{
			if (m->has_storage_semantic(static_cast<mesh_storage_semantic>(XNG_MESH_STORAGE_TEXCOORDS0 << i)) &&
				pMesh->HasTextureCoords(i))
			{
				std::transform(
					pMesh->mTextureCoords[i], pMesh->mTextureCoords[i] + pMesh->mNumVertices, (float2*)m->get_texcoords(i),
					[](const aiVector3D & aiTexcoords)
				{
					return float2(aiTexcoords.x, aiTexcoords.y);
				});
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

void assimp_loader::unload_mesh(mesh * m)
{
	m->clear();
}

bool assimp_loader::load_material(material * m)
{
	const char * name = m->get_name();

	unsigned int id;
	std::stringstream ss(name);
	ss >> id;

	// Look for the resource in the loaded scene

	if (ss.fail() || id >= m_scene->mNumMeshes)
	{
		return false;
	}

	m->set_default();

	aiMaterial * pMat = m_scene->mMaterials[id];

	os::path sceneDir = m_filename;
	sceneDir.remove_filename();

	if (pMat->GetTextureCount(aiTextureType_DIFFUSE))
	{
		aiString path;
		pMat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

		os::path p = path.C_Str();

		if (p.is_relative())
		{
			p = sceneDir / p;
		}
		
		p.make_unix_like();

		image_ptr texture = resource_factory::get_singleton()->create<image>(p.string().c_str());

		m->set_base_texture(texture);
	}

	if (pMat->GetTextureCount(aiTextureType_SPECULAR))
	{
		aiString path;
		pMat->GetTexture(aiTextureType_SPECULAR, 0, &path);

		os::path p = path.C_Str();

		if (p.is_relative())
		{
			p = sceneDir / p;
		}

		p.make_unix_like();

		image_ptr texture = resource_factory::get_singleton()->create<image>(p.string().c_str());

		m->set_specular_map(texture);
	}

	if (pMat->GetTextureCount(aiTextureType_NORMALS))
	{
		aiString path;
		pMat->GetTexture(aiTextureType_NORMALS, 0, &path);

		os::path p = path.C_Str();

		if (p.is_relative())
		{
			p = sceneDir / p;
		}

		p.make_unix_like();

		image_ptr texture = resource_factory::get_singleton()->create<image>(p.string().c_str());

		m->set_normal_map(texture);
	}

	aiColor3D baseColor, emissive;

	if (pMat->Get("$mat.baseColor", 0, 0, baseColor) == aiReturn_SUCCESS ||
	    pMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == aiReturn_SUCCESS)
	{
		m->set_base_color(reinterpret_cast<float3&>(baseColor));
	}

	return true;
}

void assimp_loader::unload_material(material * m)
{
	m->set_default();
}

#endif