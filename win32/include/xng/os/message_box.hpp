#pragma once

enum xng_message_box_type
{
    XNG_MESSAGE_BOX_DEFAULT,
    XNG_MESSAGE_BOX_ERROR,
    XNG_MESSAGE_BOX_ALERT
};

namespace xng
{
    namespace os
    {
        class native_window;

        void message_box(const char * caption, const char * message, xng_message_box_type type = XNG_MESSAGE_BOX_DEFAULT, xng::os::native_window * parent = nullptr);
        void message_box(const wchar_t * caption, const wchar_t * message, xng_message_box_type type = XNG_MESSAGE_BOX_DEFAULT, xng::os::native_window * parent = nullptr);
    }
}