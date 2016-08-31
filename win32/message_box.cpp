#include <xng/os/message_box.hpp>
#include <xng/os/native_window.hpp>

using namespace xng::os;

void xng::os::message_box(const char * caption, const char * message, xng_message_box_type type, native_window * parent)
{
	UINT mbType = MB_OK;

	switch (type)
	{

	case XNG_MESSAGE_BOX_DEFAULT:
	default:
		mbType |= MB_ICONASTERISK;
		break;

	case XNG_MESSAGE_BOX_ERROR:
		mbType |= MB_ICONERROR;
		break;

	case XNG_MESSAGE_BOX_ALERT:
		mbType |= MB_ICONWARNING;
		break;

	}

	MessageBoxA(parent ? parent->get_native_handle() : NULL, message, caption, mbType);
}

void xng::os::message_box(const wchar_t * caption, const wchar_t * message, xng_message_box_type type, native_window * parent)
{
	UINT mbType = MB_OK;

	switch (type)
	{

	case XNG_MESSAGE_BOX_DEFAULT:
	default:
		mbType |= MB_ICONASTERISK;
		break;

	case XNG_MESSAGE_BOX_ERROR:
		mbType |= MB_ICONERROR;
		break;

	case XNG_MESSAGE_BOX_ALERT:
		mbType |= MB_ICONWARNING;
		break;

	}

	MessageBoxW(parent ? parent->get_native_handle() : NULL, message, caption, mbType);
}