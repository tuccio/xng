#include <xng/graphics/acceleration_library.hpp>

using namespace xng::graphics;
using namespace xng::os;

acceleration_library::acceleration_library(void)
{
    os::shared_library accelLib;

    if (!accelLib.load("xngcugraphics.dll") &&
        !accelLib.load("xngcpugraphics.dll"))
    {
        XNG_LOG("XNG Graphics", "Unable to load acceleration library.");
    }
    else
    {
        m_library.swap(accelLib);
    }
}

shared_library::native_symbol_handle acceleration_library::get_symbol_address(const char * name)
{
    assert(m_library && "No acceleration library loaded for XNG Graphics.");
    return m_library.get_symbol_address(name);
}