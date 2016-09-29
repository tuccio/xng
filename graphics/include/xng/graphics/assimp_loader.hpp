#pragma once

#ifdef XNG_ASSIMP

#include <xng/graphics/scene.hpp>
#include <xng/graphics/mesh.hpp>
#include <xng/res.hpp>
#include <xng/os/path.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace xng
{
	namespace graphics
	{
		class assimp_loader :
			public res::resource_loader
		{

		public:

			assimp_loader(const os::path & filename, unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
			~assimp_loader(void);

			bool load(res::resource * r, const void * userdata) override;
			void unload(res::resource * r) override;

			mesh_ptr create_mesh(unsigned int meshIndex);

			const aiScene * get_scene(void) const;

			inline bool is_loaded(void) const { return m_scene != nullptr; }
			inline const char * get_error_string(void) const { return m_importer.GetErrorString(); }

		private:

			os::path           m_filename;
			const aiScene    * m_scene;
			Assimp::Importer   m_importer;

			bool load_mesh(mesh * m);
			void unload_mesh(mesh * m);

		};
	}
}

#endif