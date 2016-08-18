#include <xng/win32/message_box.hpp>
#include <xng/win32/native_window.hpp>

void xng::win32::message_box(const char * caption, const char * message, os::message_box_type type, native_window * parent)
{
	UINT mbType = MB_OK;

	switch (type)
	{

	case os::MESSAGE_BOX_DEFAULT:
	default:
		mbType |= MB_ICONASTERISK;
		break;

	case os::MESSAGE_BOX_ERROR:
		mbType |= MB_ICONERROR;
		break;

	case os::MESSAGE_BOX_ALERT:
		mbType |= MB_ICONWARNING;
		break;

	}

	MessageBoxA(parent ? parent->get_native_handle() : NULL, message, caption, mbType);
}