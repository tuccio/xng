#pragma once

#include <xng/os/types.hpp>

namespace xng
{
	namespace os
	{

		class native_window;

		void message_box(const char * caption, const char * message, xng::os::message_box_type type = os::MESSAGE_BOX_DEFAULT, xng::os::native_window * parent = nullptr);
		void message_box(const wchar_t * caption, const wchar_t * message, xng::os::message_box_type type = os::MESSAGE_BOX_DEFAULT, xng::os::native_window * parent = nullptr);
	}
}