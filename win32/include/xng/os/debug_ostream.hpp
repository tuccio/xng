#pragma once

#include <ostream>

#include <xng/os/win32_headers.hpp>

namespace xng
{
	namespace os
	{
		class debug_streambuf :
			public std::basic_streambuf<char>
		{

			std::streamsize xsputn(const char_type * s, std::streamsize n) override
			{
				OutputDebugStringA(s);
				return n;
			}

			int_type overflow(int_type c) override
			{
				char_type s[] = { c, '\0' };
				OutputDebugStringA(s);
				return 0;
			}

		};

		class debug_ostream :
			public std::basic_ostream<char>
		{

		public:

			debug_ostream(void) : std::basic_ostream<char>(&m_streambuf) { }

		private:

			debug_streambuf m_streambuf;

		};
	}
}