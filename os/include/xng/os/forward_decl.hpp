#pragma once

#include <xng/os/types.hpp>
#include <xng/core/function_alias.hpp>

#ifdef XNG_WIN32

#include <xng/win32/forward_decl.hpp>
#include <xng/win32/message_box.hpp>

namespace xng
{
	namespace os
	{
		using window    = xng::win32::native_window;
		using main_loop = xng::win32::main_loop;

		using native_window_handle = xng::win32::native_window_handle;

		using gl_context = xng::win32::wgl_context;

		typedef void(*main_loop_idle_cb) ();

		XNG_CORE_FUNCTION_ALIAS(win32::message_box, message_box)
	}
}

#endif