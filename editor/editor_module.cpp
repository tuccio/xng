#include "editor_module.hpp"

#include <xng/os.hpp>

using namespace xng::editor;
using namespace xng::engine;
using namespace xng::os;

const char *          editor_module::module_name        = "xngeditor";
const char *          editor_module::module_description = "XNG Editor Module";
const xng_module_type editor_module::module_type        = XNG_MODULE_TYPE_RUNTIME;

bool editor_module::init(void)
{
	shared_library process;
	//xng::engine::game * instance = xng_game_instance();
	xng::engine::game::get_singleton()->get_window()->show();
	//instance->get_window()->show();
	return true;
}

void editor_module::shutdown(void)
{
}

bool editor_module::is_initialized(void) const
{
	return false;
}

void editor_module::update(float dt)
{
}