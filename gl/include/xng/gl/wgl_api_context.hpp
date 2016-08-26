#pragma once

#ifdef XNG_WIN32

#include <xng/win32/win32_headers.hpp>
#include <xng/gl/gl_headers.hpp>
#include <xng/gl/gl_api_context.hpp>

#include <xng/graphics/api_version.hpp>

namespace xng
{
	namespace gl
	{
		class wgl_api_context :
			public gl_api_context
		{

		public:

			wgl_api_context(void);
			wgl_api_context(const wgl_api_context &) = delete;
			wgl_api_context(wgl_api_context &&);

			~wgl_api_context(void) override;

			bool init(HWND hWnd, xng_api_version version = XNG_API_DEFAULT, bool debug = false) override;
			void shutdown(void) override;

			bool is_initialized(void) const override;

			void use(void) override;
			void dispose(void) override;

			void frame_complete(void) override;

			void set_vsync(bool vsync) override;
			bool get_vsync(void) const override;

			bool is_supported(const char * extension) override;

			void make_current(void) const;
			void release_current(void) const;

			void swap_buffers(void) const;

		private:

			HDC   m_hDC;
			HGLRC m_hGLRC;

		};
	}
}

#endif