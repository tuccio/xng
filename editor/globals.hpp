#pragma once

#include <xng/graphics.hpp>
#include <memory>

namespace xng
{
	namespace editor
	{
		extern std::unique_ptr<graphics::api_context> g_apiContext;
		extern std::unique_ptr<graphics::scene>       g_scene;
		extern std::unique_ptr<graphics::renderer>    g_renderer;
	}
}