#include <xng/os.hpp>
#include <xng/graphics.hpp>
#include <xng/res.hpp>
#include <xng/engine.hpp>

template struct xng::core::singleton<xng::core::logger>;
template struct xng::core::singleton<xng::res::resource_factory>;
template struct xng::core::singleton<xng::engine::module_manager>;
template struct xng::core::singleton<xng::engine::game>;