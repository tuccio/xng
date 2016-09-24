#pragma once

#include <xng/os/win32_headers.hpp>
#include <xng/os/native_window_observer.hpp>

#include <xng/core.hpp>
#include <xng/math.hpp>

#include <string>

namespace xng
{
	namespace os
	{

		class native_window :
			public core::observable<native_window_observer, std::mutex>
		{

		public:

			typedef HWND native_handle;

			native_window(void);
			native_window(const native_window &) = delete;
			native_window(native_window &&);

			~native_window(void);

			bool create(void);
			void destroy(void);

			bool exists(void) const;

			void show(void);
			void hide(void);

			void set_caption(const char * title);
			std::string get_caption(void) const;

			void set_borderless(bool borderless);
			bool is_borderless(void) const;

			void set_fullscreen(bool fullscreen);
			bool is_fullscreen(void) const;

			void set_taskbar_icon(bool taskbar);
			bool has_taskbar_icon(void) const;

			native_handle get_native_handle(void);

			math::uint2 get_position(void) const;
			void set_position(const math::uint2 & position);

			math::uint2 get_window_size(void) const;
			void set_window_size(const math::uint2 & size);

			math::uint2 get_client_size(void) const;
			void set_client_size(const math::uint2 & size);

		private:

			HWND m_hWnd;

			static LRESULT CALLBACK wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			static bool is_class_registered(void);
			static bool register_class(void);
			static void unregister_class(void);

			static native_window * get_window_object(native_handle hWnd);

		};

		typedef native_window::native_handle native_window_handle;

	}
}