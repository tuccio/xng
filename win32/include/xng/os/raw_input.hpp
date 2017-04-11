#pragma once

#include <xng/os/win32_headers.hpp>
#include <xng/input/vkeys.hpp>

namespace xng
{
    namespace os
    {
        void translate_keyboard_key(const RAWKEYBOARD & rawKeyboard, xng_keyboard_key & xngKey, bool & keyUp);
    }
}