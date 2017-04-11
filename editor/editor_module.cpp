#include "editor_module.hpp"

#include <xng/os.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

using namespace xng::editor;
using namespace xng::engine;
using namespace xng::os;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;

const char *          editor_module::module_name        = "xngeditor";
const char *          editor_module::module_description = "XNG Editor Module";
const xng_module_type editor_module::module_type        = XNG_MODULE_TYPE_RUNTIME;

bool editor_module::init(void)
{
    m_editor = std::make_unique<editor>(game::get_singleton()->get_window());
    return true;
}

void editor_module::shutdown(void)
{
    m_editor.reset();
}

bool editor_module::is_initialized(void) const
{
    return (bool) m_editor;
}

void editor_module::update(float dt)
{
    m_editor->update(dt);
}