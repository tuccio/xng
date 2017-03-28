#include <xng/dx11/debug_renderer.hpp>

#include <xng/geometry/debug.hpp>

using namespace xng::dx11;
using namespace xng::math;
using namespace xng::graphics;

struct cb_per_frame
{
	float4x4 viewProjectionMatrix;
};

bool debug_renderer::init(dx11_api_context * context, const render_variables & rvars)
{
	m_apiContext = context;
	m_vbSize     = 0;
	return m_program.preprocess(XNG_DX11_SHADER_FILE("debug.xhlsl")) &&
	       m_cbPerFrame.create<cb_per_frame>(context->get_device());
}

void debug_renderer::shutdown(void)
{
	m_program.clear();
}

void debug_renderer::render_begin(ID3D11DeviceContext * context, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const camera * cam)
{
	m_context = context;
	m_rtv     = rtv;
	m_dsv     = dsv;

	cb_per_frame data = { cam->get_directx_projection_matrix() * cam->get_directx_view_matrix() };
	m_cbPerFrame.write(context, &data);
	context->VSSetConstantBuffers(0, 1, &m_cbPerFrame);
	
	m_lines.clear();
}

void debug_renderer::render_end(void)
{
	size_t requestedSize = m_lines.size() * sizeof(vertex);

	if (requestedSize > 0)
	{
		if (requestedSize > m_vbSize)
		{
			XNG_HR_CHECK(m_apiContext->get_device()->CreateBuffer(
				&CD3D11_BUFFER_DESC(requestedSize, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
				nullptr,
				m_vb.reset_and_get_address()));
		}

		m_context->OMSetRenderTargets(1, &m_rtv, m_dsv.get());
	
		shader_program program = m_program.compile(m_apiContext->get_device());

		program.use(m_context.get());

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (!XNG_HR_FAILED(m_context->Map(m_vb.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, m_lines.data(), requestedSize);

			m_context->Unmap(m_vb.get(), 0);

			m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			UINT strides[] = { sizeof(vertex) };
			UINT offsets[] = { 0 };

			m_context->IASetVertexBuffers(0, 1, &m_vb, strides, offsets);
			m_context->Draw(m_lines.size(), 0);
		}

		program.dispose(m_context.get());
	}

	m_rtv.reset();
	m_dsv.reset();
}

void debug_renderer::render(const aabb & bb, const float4 & color)
{
	auto corners = bb.get_corners();

	// Back

	render(corners[XNG_AABB_BACK_BOTTOM_LEFT],  corners[XNG_AABB_BACK_BOTTOM_RIGHT], color);
	render(corners[XNG_AABB_BACK_BOTTOM_RIGHT], corners[XNG_AABB_BACK_TOP_RIGHT], color);
	render(corners[XNG_AABB_BACK_TOP_RIGHT],    corners[XNG_AABB_BACK_TOP_LEFT], color);
	render(corners[XNG_AABB_BACK_TOP_LEFT],     corners[XNG_AABB_BACK_BOTTOM_LEFT], color);

	// Front

	render(corners[XNG_AABB_FRONT_BOTTOM_LEFT],  corners[XNG_AABB_FRONT_BOTTOM_RIGHT], color);
	render(corners[XNG_AABB_FRONT_BOTTOM_RIGHT], corners[XNG_AABB_FRONT_TOP_RIGHT], color);
	render(corners[XNG_AABB_FRONT_TOP_RIGHT],    corners[XNG_AABB_FRONT_TOP_LEFT], color);
	render(corners[XNG_AABB_FRONT_TOP_LEFT],     corners[XNG_AABB_FRONT_BOTTOM_LEFT], color);

	// Link

	render(corners[XNG_AABB_FRONT_BOTTOM_LEFT],  corners[XNG_AABB_BACK_BOTTOM_LEFT], color);
	render(corners[XNG_AABB_FRONT_BOTTOM_RIGHT], corners[XNG_AABB_BACK_BOTTOM_RIGHT], color);
	render(corners[XNG_AABB_FRONT_TOP_RIGHT],    corners[XNG_AABB_BACK_TOP_RIGHT], color);
	render(corners[XNG_AABB_FRONT_TOP_LEFT],     corners[XNG_AABB_BACK_TOP_LEFT], color);
}

void debug_renderer::render(const frustum & f, const float4 & color)
{
	auto corners = f.get_corners();

	// Near

	render(corners[XNG_FRUSTUM_NEAR_BOTTOM_LEFT],  corners[XNG_FRUSTUM_NEAR_BOTTOM_RIGHT], color);
	render(corners[XNG_FRUSTUM_NEAR_BOTTOM_RIGHT], corners[XNG_FRUSTUM_NEAR_TOP_RIGHT], color);
	render(corners[XNG_FRUSTUM_NEAR_TOP_RIGHT],    corners[XNG_FRUSTUM_NEAR_TOP_LEFT], color);
	render(corners[XNG_FRUSTUM_NEAR_TOP_LEFT],     corners[XNG_FRUSTUM_NEAR_BOTTOM_LEFT], color);

	//// Far

	render(corners[XNG_FRUSTUM_FAR_BOTTOM_LEFT],  corners[XNG_FRUSTUM_FAR_BOTTOM_RIGHT], color);
	render(corners[XNG_FRUSTUM_FAR_BOTTOM_RIGHT], corners[XNG_FRUSTUM_FAR_TOP_RIGHT], color);
	render(corners[XNG_FRUSTUM_FAR_TOP_RIGHT],    corners[XNG_FRUSTUM_FAR_TOP_LEFT], color);
	render(corners[XNG_FRUSTUM_FAR_TOP_LEFT],     corners[XNG_FRUSTUM_FAR_BOTTOM_LEFT], color);

	// Link

	render(corners[XNG_FRUSTUM_NEAR_BOTTOM_LEFT],  corners[XNG_FRUSTUM_FAR_BOTTOM_LEFT], color);
	render(corners[XNG_FRUSTUM_NEAR_BOTTOM_RIGHT], corners[XNG_FRUSTUM_FAR_BOTTOM_RIGHT], color);
	render(corners[XNG_FRUSTUM_NEAR_TOP_RIGHT],    corners[XNG_FRUSTUM_FAR_TOP_RIGHT], color);
	render(corners[XNG_FRUSTUM_NEAR_TOP_LEFT],     corners[XNG_FRUSTUM_FAR_TOP_LEFT], color);

	write_octave_plot_script(f, "C:\\Users\\Davide\\Desktop\\frustum.m");
	write_octave_plot_script(corners.begin(), corners.end(), "C:\\Users\\Davide\\Desktop\\frustum_corners.m");
}


void debug_renderer::render(const float3 & x0, const float3 & x1, const float4 & color)
{
	m_lines.push_back(vertex{ x0, color });
	m_lines.push_back(vertex{ x1, color });
}
