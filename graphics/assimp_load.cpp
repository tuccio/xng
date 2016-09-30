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

void create_scene_graph_assimp(assimp_loader * loader, aiNode * node, scene * scene, scene_graph_node * parent)
{
	if (node)
	{
		const aiScene * assimpScene = loader->get_scene();

		scene_graph_node * newNode = nullptr;
		aiCamera * aiCam = nullptr;

		if (node->mNumMeshes)
		{
			newNode = parent->add_child<scene_graph_geometry>();
		}
		else if (aiCam = find_camera(assimpScene, node))
		{
			newNode = parent->add_child<scene_graph_camera>();
		}
		else
		{
			newNode = parent->add_child<scene_graph_group>();
		}

		if (newNode)
		{
			float4x4 m = transpose(reinterpret_cast<const float4x4&>(node->mTransformation));

			quaternion r;
			float3 s, t;
			decompose_affine(m, &s, &r, &t);

			r = normalize(r);

			newNode->set_name(node->mName.C_Str());

			newNode->set_local_rotation(r);
			newNode->set_local_translation(t);
			newNode->set_local_scale(s);

			newNode->set_name(node->mName.C_Str());

			for (int i = 0; i < node->mNumChildren; ++i)
			{
				create_scene_graph_assimp(loader, node->mChildren[i], scene, newNode);
			}

			switch (newNode->get_type())
			{

			case XNG_SCENE_GRAPH_GEOMETRY:
			{
				scene_graph_geometry * gNode = static_cast<scene_graph_geometry*>(newNode);

				unsigned int meshIndex = node->mMeshes[0];
				unsigned int materialIndex = assimpScene->mMeshes[meshIndex]->mMaterialIndex;

				mesh_ptr nodeMesh = loader->create_mesh(meshIndex);

				if (nodeMesh && nodeMesh->load())
				{
					if (!nodeMesh->has_storage_semantic(XNG_MESH_STORAGE_TANGENTS))
					{
						nodeMesh->calculate_tangent_space();
					}

					gNode->set_mesh(nodeMesh);
				}

				break;
			}

			case XNG_SCENE_GRAPH_CAMERA:
			{
				scene_graph_camera * cNode = static_cast<scene_graph_camera*>(newNode);

				float3 position      = reinterpret_cast<const float3&>(aiCam->mPosition);
				float3 up            = reinterpret_cast<const float3&>(aiCam->mUp);
				float3 viewDirection = reinterpret_cast<const float3&>(aiCam->mLookAt);

				//position.z = -position.z;

				float3 lookAt = viewDirection - position;

				camera * cam = cNode->get_camera();

				cam->look_at(position, up, lookAt);

				cam->set_znear(aiCam->mClipPlaneNear);
				cam->set_zfar(aiCam->mClipPlaneFar);

				cNode->set_local_rotation(cam->get_orientation());
				cNode->set_local_translation(cam->get_position());

				break;
			}
			}
		}
	}
}

bool xng::graphics::assimp_load(scene * scene, const path & filename)
{
	refcounted_ptr<assimp_loader> loader = xng_new assimp_loader(filename);

	if (!loader->is_loaded())
	{
		return false;
	}

	create_scene_graph_assimp(loader.get(), loader->get_scene()->mRootNode, scene, scene->get_scene_graph()->get_root());

	return true;
}

#endif
