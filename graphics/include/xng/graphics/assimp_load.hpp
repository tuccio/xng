#pragma once

#ifdef XNG_ASSIMP

#include <xng/graphics/scene.hpp>
#include <xng/os/path.hpp>

#include <assimp/postprocess.h>

namespace xng
{
	namespace graphics
	{
		bool assimp_load(scene * scene, const os::path & filename, int flags = aiProcessPreset_TargetRealtime_MaxQuality);
	}
}


#endif