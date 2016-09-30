#pragma once

#include <xng/graphics/api_context.hpp>
#include <xng/graphics/api_version.hpp>

#include <xng/dx11/dx11_headers.hpp>
#include <xng/dx11/gpu_profiler.hpp>

namespace xng
{
	namespace dx11
	{
		class dx11_api_context :
			public graphics::api_context
		{

		public:

			bool init(os::native_window_handle handle, xng_api_version version = XNG_API_DEFAULT, bool debug = false) override;
			bool init(os::native_window_handle handle, xng_api_version version, bool debug, DXGI_SWAP_CHAIN_DESC * swapChainDesc);
			void shutdown(void) override;

			bool is_initialized(void) const override;

			void frame_start(void) override;
			void frame_complete(void) override;

			void on_resize(uint32_t width, uint32_t height) override;

			ID3D11Device * get_device(void);
			ID3D11DeviceContext * get_immediate_context(void);

			ID3D11RenderTargetView * get_back_buffer_rtv(void);

			DXGI_SAMPLE_DESC get_msaa_sample_desc(DXGI_FORMAT format, uint32_t msaa) const;

			void set_vsync(bool vsync) override;
			bool get_vsync(void) const override;

			void profile_start(const char * name) override;
			void profile_complete(const char * name) override;

			graphics::profiler_data get_profiler_data(void) override;

		private:

			com_ptr<ID3D11Device>           m_device;
			com_ptr<ID3D11DeviceContext>    m_immediateContext;
			com_ptr<IDXGISwapChain>         m_swapChain;
			com_ptr<ID3D11RenderTargetView> m_backBufferRTV;

			gpu_profiler            m_profiler;
			graphics::profiler_data m_profilerData;

			DXGI_FORMAT m_backBufferFormat;
			bool m_vsync;

			bool create_back_buffer_view(void);

		};
	}
}