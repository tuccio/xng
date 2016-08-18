#include <xng/win32/errors.hpp>

#include <Windows.h>

std::string xng::win32::get_last_error(void)
{
	DWORD error = GetLastError();

	LPSTR output;

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		(LPSTR)&output,
		0x0,
		nullptr);

	std::string s = output;

	LocalFree((HLOCAL)output);

	return s;
}