#ifdef XNG_ASSIMP

#include <xng/graphics/assimp_load.hpp>
#include <xng/graphics/assimp_loader.hpp>

using namespace xng::core;
using namespace xng::graphics;
using namespace xng::os;
using namespace xng::res;
using namespace xng::math;

aiCamera * find_camera(const aiScene * scene, aiNode * node)
{
	for (int i = 0; i < scene->mNumCameras; ++i)
	{
		if (scene->mCameras[i]->mName == node->mName)
		{
			return scene->mCameras[i];
		}
	}

	return nullptr;
}

aiLight * find_light(const aiScene * scene, aiNode * node)
{
	for (int i = 0; i < scene->mNumLights; ++i)
	{
		if (scene->mLights[i]->mName == node->mName)
		{
			return scene->mLights[i];
		}
	}

	return nullptr;
}

void create_scene_graph_assimp(assimp_loader * loader, aiNode * node, scene * scene, scene_graph_node * parent)
{
	if (node)
	{
		const aiScene * assimpScene = loader->get_scene();

		aiCamera * aiCam   = nullptr;
		aiLight  * aiLight = nullptr;

		float4x4 m;
		quaternion r;
		float3 s, t;

		m = transpose(reinterpret_cast<const float4x4&>(node->mTransformation));
		decompose_affine(m, &s, &r, &t);
		r = normalize(r);

		scene_graph_group * groupNode = parent->add_child<scene_graph_group>();

		groupNode->set_local_rotation(r);
		groupNode->set_local_translation(t);
		groupNode->set_local_scale(s);

		groupNode->set_name(node->mName.C_Str());
		
		for (int i = 0; i < node->mNumMeshes; ++i)
		{
			scene_graph_geometry * gNode = groupNode->add_child<scene_graph_geometry>();

			unsigned int meshIndex     = node->mMeshes[i];
			unsigned int materialIndex = assimpScene->mMeshes[meshIndex]->mMaterialIndex;

			mesh_ptr     nodeMesh     = loader->create_mesh(meshIndex);
			material_ptr nodeMaterial = loader->create_material(materialIndex);

			if (nodeMesh && nodeMesh->load())
			{
				if (!nodeMesh->has_storage_semantic(XNG_MESH_STORAGE_TANGENTS))
				{
					nodeMesh->calculate_tangent_space();
				}

				gNode->set_mesh(nodeMesh);
			}

			gNode->set_material(nodeMaterial);
		}

		if (aiLight = find_light(assimpScene, node))
		{
			scene_graph_light * lNode = groupNode->add_child<scene_graph_light>();

			float3 direction = reinterpret_cast<const float3&>(aiLight->mDirection);

			float dirLength = length(direction);

			if (dirLength < 0.1f)
			{
				direction = float3(0, -1, 0);
			}
			else
			{
				direction = direction / dirLength;
			}

			float3 luminance = reinterpret_cast<const float3&>(aiLight->mColorDiffuse);
			float  intensity = max(0.f, max(max(luminance.x, luminance.y), luminance.z));

			lNode->set_color(intensity > 0.1f ? luminance / intensity : float3(0));
			lNode->set_intensity(intensity);
			lNode->set_ambient(reinterpret_cast<const float3&>(aiLight->mColorAmbient));
			lNode->set_position(reinterpret_cast<const float3&>(aiLight->mPosition));
			lNode->set_direction(direction);
			lNode->set_cutoff_angle(aiLight->mAngleInnerCone);

			switch (aiLight->mType)
			{
			case aiLightSource_DIRECTIONAL:
				lNode->set_light_type(XNG_LIGHT_DIRECTIONAL);
				break;
			case aiLightSource_POINT:
				lNode->set_light_type(XNG_LIGHT_POINT);
				break;
			case aiLightSource_SPOT:
				lNode->set_light_type(XNG_LIGHT_SPOT);
				break;
			}
		}

		if (aiCam = find_camera(assimpScene, node))
		{
			scene_graph_camera * cNode = groupNode->add_child<scene_graph_camera>();

			float3 position      = reinterpret_cast<const float3&>(aiCam->mPosition);
			float3 up            = reinterpret_cast<const float3&>(aiCam->mUp);
			float3 viewDirection = reinterpret_cast<const float3&>(aiCam->mLookAt);

			float3 lookAt = viewDirection - position;

			camera * cam = cNode->get_camera();

			cam->look_at(position, up, lookAt);

			cam->set_znear(aiCam->mClipPlaneNear);
			cam->set_zfar(aiCam->mClipPlaneFar);

			cNode->set_local_rotation(cam->get_orientation());
			cNode->set_local_translation(cam->get_position());
		}

		for (int i = 0; i < node->mNumChildren; ++i)
		{
			create_scene_graph_assimp(loader, node->mChildren[i], scene, groupNode);
		}
	}
}

bool xng::graphics::assimp_load(scene * scene, const path & filename, int flags)
{
	refcounted_ptr<assimp_loader> loader = xng_new assimp_loader(filename, flags);

	if (!loader->is_loaded())
	{
		return false;
	}

	create_scene_graph_assimp(loader.get(), loader->get_scene()->mRootNode, scene, scene->get_scene_graph()->get_root());

	return true;
}

#endif
