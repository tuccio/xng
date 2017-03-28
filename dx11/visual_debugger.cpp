#include <xng/dx11/visual_debugger.hpp>

using namespace xng::math;
using namespace xng::graphics;
using namespace xng::dx11;

bool visual_debugger::init(dx11_api_context * context, const render_variables & rvars)
{
	return m_renderer.init(context, rvars);
}

void visual_debugger::shutdown(void)
{
	m_renderer.shutdown();
}

void visual_debugger::render(ID3D11DeviceContext * deviceContext, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, const camera * camera)
{
	m_renderer.render_begin(deviceContext, rtv, dsv, camera);
	draw_persistent_objects();
	m_renderer.render_end();
}

void visual_debugger::add_persistent(const char * name, const aabb & boundingBox, const float4 & color)
{
	m_persistentObjects.emplace(name, draw_info{ boundingBox, color });
}

void visual_debugger::add_persistent(const char * name, const frustum & f, const float4 & color)
{
	m_persistentObjects.emplace(name, draw_info{ f, color });
}

void visual_debugger::remove_persistent(const char * name)
{
	auto it = m_persistentObjects.find(name);

	if (it != m_persistentObjects.end())
	{
		m_persistentObjects.erase(it);
	}
}

class persistent_object_visitor :
	public boost::static_visitor<void>
{

public:

	persistent_object_visitor(debug_renderer * r, const float4 & c) :
		m_renderer(r), m_color(c) {}

	void operator()(const aabb & boundingBox) const
	{
		m_renderer->render(boundingBox, m_color);
	}

	void operator()(const frustum & f) const
	{
		m_renderer->render(f, m_color);
	}

private:

	debug_renderer * m_renderer;
	float4           m_color;

};


void visual_debugger::draw_persistent_objects(void)
{
	for (auto & p : m_persistentObjects)
	{
		boost::apply_visitor(persistent_object_visitor{ &m_renderer, p.second.color }, p.second.geometry);
	}
}