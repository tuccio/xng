#pragma once

#include <xng/graphics/scene.hpp>
#include <xng/os/path.hpp>

#ifdef XNG_ASSIMP

namespace xng
{
	namespace graphics
	{
		bool assimp_load(scene * scene, const os::path & filename);
	}
}


#endif