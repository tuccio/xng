#include "globals.hpp"

std::unique_ptr<xng::graphics::api_context> xng::editor::g_apiContext;
std::unique_ptr<xng::graphics::scene>       xng::editor::g_scene;
std::unique_ptr<xng::graphics::renderer>    xng::editor::g_renderer;