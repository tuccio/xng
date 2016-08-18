#include <xng/dx11/forward_renderer.hpp>

#include <xng/dx11.hpp>

using namespace xng;
using namespace xng::dx11;

bool forward_renderer::init(graphics::api_context * context)
{
	m_apiContext = dynamic_cast<dx11_api_context*>(context);
	return m_apiContext != nullptr;
}

void forward_renderer::shutdown(void)
{
	m_apiContext = nullptr;
}

void forward_renderer::render(graphics::scene * scene, graphics::camera * camera, graphics::render_resource * target)
{
	graphics::render_variables rvars;
	process_rv_updates(&rvars);

	ID3D11DeviceContext * ctx = m_apiContext->get_immediate_context();

	float clearColor[4] = { 0 };

	ctx->ClearRenderTargetView(m_apiContext->get_back_buffer_rtv(), clearColor);
}

void forward_renderer::process_rv_updates(graphics::render_variables * rvars)
{
	std::set<xng_render_variable> updates;
	get_render_variables(rvars, &updates);
}