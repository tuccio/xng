#include <xng/os/win32_headers.hpp>
#include <xng/os/filesystem.hpp>

#include <cstring>

namespace xng
{
	namespace os
	{
		bool exists(const path & p)
		{
			WIN32_FIND_DATAW ffd;
			HANDLE hFind = FindFirstFileW(p.c_str(), &ffd);

			bool exists = hFind != INVALID_HANDLE_VALUE;

			if (exists)
			{
				FindClose(hFind);
			}

			return exists;
		}

		bool is_directory(const path & p)
		{
			WIN32_FIND_DATAW ffd;
			HANDLE hFind = FindFirstFileW(p.c_str(), &ffd);

			bool exists = hFind != INVALID_HANDLE_VALUE;
			bool isDir  = false;

			if (exists)
			{
				isDir = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				FindClose(hFind);
			}

			return exists;
		}

		bool is_regular_file(const path & p)
		{
			WIN32_FIND_DATAW ffd;
			HANDLE hFind = FindFirstFileW(p.c_str(), &ffd);

			bool exists = hFind != INVALID_HANDLE_VALUE;
			bool isFile = false;

			if (exists)
			{
				isFile = (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
				FindClose(hFind);
			}

			return exists;
		}

		path current_directory(void)
		{
			DWORD size = GetCurrentDirectoryW(0, nullptr);
			std::wstring buffer(size, 0);
			GetCurrentDirectoryW(buffer.size(), &buffer[0]);
			return path(buffer);
		}

		void change_current_directory(const path & p)
		{
			SetCurrentDirectoryW(p.c_str());
		}
	}
}