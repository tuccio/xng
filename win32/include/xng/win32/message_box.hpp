#pragma once

#include <xng/win32/forward_decl.hpp>
#include <xng/os/forward_decl.hpp>

namespace xng
{
	namespace win32
	{
		void message_box(const char * caption, const char * message, xng::os::message_box_type type = os::MESSAGE_BOX_DEFAULT, native_window * parent = nullptr);
	}
}