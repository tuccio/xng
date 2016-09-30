#include <xng/dx11/dx11_api_context.hpp>

using namespace xng::dx11;
using namespace xng::graphics;

bool dx11_api_context::init(native_window_handle handle, xng_api_version version, bool debug)
{
	DXGI_SWAP_CHAIN_DESC swapDesc        = {};

	swapDesc.BufferDesc.RefreshRate      = { 1, 60 };

	swapDesc.BufferDesc.Width            = 1;
	swapDesc.BufferDesc.Height           = 1;

	swapDesc.BufferDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;

	swapDesc.BufferCount                 = 1;
	swapDesc.BufferUsage                 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow                = handle;
	swapDesc.Windowed                    = TRUE;
	swapDesc.SwapEffect                  = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags                       = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	swapDesc.SampleDesc                  = { 1, 0 };

	return init(handle, version, debug, &swapDesc);
}

bool dx11_api_context::init(native_window_handle handle, xng_api_version version, bool debug, DXGI_SWAP_CHAIN_DESC * swapChainDesc)
{
	if (!swapChainDesc)
	{
		return init(handle, version, debug);
	}

	m_backBufferFormat = swapChainDesc->BufferDesc.Format;

	UINT createDeviceFlags = debug ? D3D11_CREATE_DEVICE_DEBUG : 0x0;
	D3D_FEATURE_LEVEL featureLevel;

	if (!XNG_HR_FAILED(
		D3D11CreateDevice(
			0x0,
			D3D_DRIVER_TYPE_HARDWARE,
			0x0,
			createDeviceFlags,
			0x0,
			0x0,
			D3D11_SDK_VERSION,
			&m_device,
			&featureLevel,
			&m_immediateContext)))
	{
		swapChainDesc->SampleDesc = get_msaa_sample_desc(swapChainDesc->BufferDesc.Format, swapChainDesc->SampleDesc.Count);

		com_ptr<IDXGIDevice>  dxgiDevice;
		com_ptr<IDXGIAdapter> dxgiAdapter;
		com_ptr<IDXGIFactory> dxgiFactory;

		return
			m_profiler.init(m_device.get(), swapChainDesc->BufferCount + 1) &&
			!XNG_HR_FAILED(m_device->QueryInterface(IID_PPV_ARGS(&dxgiDevice))) &&
			!XNG_HR_FAILED(dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter))) &&
			!XNG_HR_FAILED(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))) &&
			!XNG_HR_FAILED(dxgiFactory->CreateSwapChain(m_device.get(), swapChainDesc, &m_swapChain)) &&
			create_back_buffer_view();
	}

	return false;
}

void dx11_api_context::shutdown(void)
{
	m_backBufferRTV.reset();
	m_immediateContext.reset();
	m_device.reset();
	m_swapChain.reset();
}

bool dx11_api_context::is_initialized(void) const
{
	return (bool) m_device;
}

void dx11_api_context::frame_start(void)
{
	m_profiler.frame_start(m_immediateContext.get());
}

void dx11_api_context::frame_complete(void)
{
	m_swapChain->Present(m_vsync ? 1 : 0, 0);
	m_profiler.frame_complete(m_immediateContext.get());
	m_profilerData = m_profiler.collect_data(m_immediateContext.get());
}

void dx11_api_context::on_resize(uint32_t width, uint32_t height)
{
	if (m_device)
	{
		m_backBufferRTV.reset();
		if (!XNG_HR_FAILED(m_swapChain->ResizeBuffers(1, width, height, m_backBufferFormat, 0)))
		{
			create_back_buffer_view();
		}
	}
}

bool dx11_api_context::create_back_buffer_view(void)
{
	com_ptr<ID3D11Texture2D> backBuffer;
	m_backBufferRTV.reset();
	return
		!XNG_HR_FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) &&
		!XNG_HR_FAILED(m_device->CreateRenderTargetView(backBuffer.get(), nullptr, &m_backBufferRTV));
}

ID3D11Device * dx11_api_context::get_device(void)
{
	return m_device.get();
}

ID3D11DeviceContext * dx11_api_context::get_immediate_context(void)
{
	return m_immediateContext.get();
}

ID3D11RenderTargetView * dx11_api_context::get_back_buffer_rtv(void)
{
	return m_backBufferRTV.get();
}

DXGI_SAMPLE_DESC dx11_api_context::get_msaa_sample_desc(DXGI_FORMAT format, uint32_t msaa) const
{
	DXGI_SAMPLE_DESC sampleDesc;
	UINT quality;

	if (!XNG_HR_FAILED(m_device->CheckMultisampleQualityLevels(format, msaa, &quality)))
	{
		sampleDesc.Count   = msaa;
		sampleDesc.Quality = quality - 1;
	}
	else
	{
		sampleDesc.Count   = 1;
		sampleDesc.Quality = 0;
	}

	return sampleDesc;
}

void dx11_api_context::set_vsync(bool vsync)
{
	m_vsync = vsync;
}

bool dx11_api_context::get_vsync(void) const
{
	return m_vsync;
}

void dx11_api_context::profile_start(const char * name)
{
	m_profiler.profile_start(name, m_immediateContext.get());
}

void dx11_api_context::profile_complete(const char * name)
{
	m_profiler.profile_complete(name, m_immediateContext.get());
}

profiler_data dx11_api_context::get_profiler_data(void)
{
	return m_profilerData;
}