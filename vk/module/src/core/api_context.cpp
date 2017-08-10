#include <xng/vk/core/api_context.hpp>

using namespace xng::vk;
using namespace xng;

bool api_context::init(os::native_window_handle handle, xng_api_version version, bool debug)
{
	return false;
}

void api_context::shutdown(void)
{

}

bool api_context::is_initialized(void) const
{
	return false;
}

void api_context::frame_complete(void)
{

}

void api_context::set_vsync(bool vsync)
{

}

bool api_context::get_vsync(void) const
{
	return true;
}