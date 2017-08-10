#include "vktests_module.hpp"

#include <xng/os.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>

using namespace xng::engine;
using namespace xng::os;
using namespace xng::graphics;
using namespace xng::res;
using namespace xng::math;
using namespace xng::vk::test;

const char *          vktests_module::module_name        = "xngvktests";
const char *          vktests_module::module_description = "XNG Editor Module";
const xng_module_type vktests_module::module_type        = XNG_MODULE_TYPE_RUNTIME;

bool vktests_module::init(void)
{
	m_testsWindow = std::make_unique<tests_window>(game::get_singleton()->get_window());
    return true;
}

void vktests_module::shutdown(void)
{
	m_testsWindow.reset();
}

bool vktests_module::is_initialized(void) const
{
    return (bool) m_testsWindow;
}

void vktests_module::update(float dt)
{
	m_testsWindow->update(dt);
}