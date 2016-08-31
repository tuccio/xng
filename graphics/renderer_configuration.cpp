#include <xng/graphics/renderer_configuration.hpp>

using namespace xng;
using namespace xng::graphics;

renderer_configuration::renderer_configuration(void)
{
	m_vars = make_default_render_variables();
}

void renderer_configuration::get_render_variables(render_variables * vars, std::set<xng_render_variable> * updates)
{
	*vars    = m_vars;
	*updates = std::move(m_updates);
}