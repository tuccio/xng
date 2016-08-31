#pragma once

#include <xng/os/path.hpp>
#include <xng/os/win32_headers.hpp>

namespace xng
{
	namespace os
	{
		struct shared_library
		{

		public:

			typedef HMODULE native_handle;
			typedef FARPROC native_symbol_handle;

			static const path::value_type * extension;

			shared_library(void);
			shared_library(const shared_library &) = delete;
			shared_library(shared_library &&);

			~shared_library(void);

			bool load(const path & p);
			void unload(void);
			bool is_loaded(void) const;
			operator bool() const;

			native_symbol_handle get_symbol_address(const char * name);

			void swap(shared_library & rhs);

			native_handle release(void);

		private:

			native_handle m_hModule;

		};
	}
}