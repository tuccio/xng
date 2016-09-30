#include <xng/dx11/render_resource_manager.hpp>

#include <algorithm>
#include <sstream>

using namespace xng::dx11;
using namespace xng::dx11::detail;

static inline uint64_t compute_ordering_key(const render_resource_wrapper & rrw)
{
	// [ ..., DXGI_FORMAT (8 bits), Width (13 bits), Height (13 bits) ]

	uint64_t key;

	key = (rrw.description.Height & 0x1FFFULL);
	key |= (rrw.description.Width & 0x1FFFULL) << 13;
	key |= (rrw.description.Format & 0xFFULL) << 26;

	return key;
}

render_resource_wrapper * render_resource_manager::find_texture_2d(const D3D11_TEXTURE2D_DESC & description)
{
	render_resource_wrapper resourceWrapper = { description, 0, false };

	auto eqr = m_descriptionMap.equal_range(compute_ordering_key(resourceWrapper));

	for (auto it = eqr.first; it != eqr.second; it++)
	{
		render_resource_wrapper * pIt = it->second;

		assert(pIt->description.Width == description.Width &&
			pIt->description.Height == description.Height &&
			pIt->description.Format == description.Format &&
			"The retrieved resource doesn't match with the one requested (this most likely indicates a bug in the key ordering).");

		if (pIt->usageFlag == false &&
			(description.BindFlags & pIt->description.BindFlags) == description.BindFlags &&
			description.SampleDesc.Count == pIt->description.SampleDesc.Count &&
			description.SampleDesc.Quality == pIt->description.SampleDesc.Quality &&
			description.ArraySize == pIt->description.ArraySize &&
			description.MipLevels == pIt->description.MipLevels)
		{
			return pIt;
		}

	}

	return nullptr;
}

render_resource_wrapper * render_resource_manager::create_texture_2d(ID3D11Device * device, const D3D11_TEXTURE2D_DESC & description)
{
	uint32_t id = m_resources.size();
	render_resource_wrapper resourceWrapper = { description, id, false };

	auto it = m_descriptionMap.upper_bound(compute_ordering_key(resourceWrapper));

	render_resource * resource = xng_new render_resource;

	if (!resource->create(device, &description))
	{
		xng_delete resource;
		return nullptr;
	}

	render_resource_wrapper * pResourceWrapper = xng_new render_resource_wrapper(resourceWrapper);

	compute_ordering_key(*pResourceWrapper);

	m_resources.emplace_back(resource, pResourceWrapper);
	m_descriptionMap.emplace(compute_ordering_key(*pResourceWrapper), pResourceWrapper);

	return pResourceWrapper;

}

render_resource_handle_ptr render_resource_manager::get_texture_2d(
	ID3D11Device * device,
	const D3D11_TEXTURE2D_DESC & desc,
	const D3D11_RENDER_TARGET_VIEW_DESC * rtvDesc,
	const D3D11_SHADER_RESOURCE_VIEW_DESC * srvDesc,
	const D3D11_UNORDERED_ACCESS_VIEW_DESC * uavDesc,
	const D3D11_DEPTH_STENCIL_VIEW_DESC * dsvDesc)
{
	render_resource_wrapper * r = find_texture_2d(desc);

	uint32_t id;

	if (!r)
	{
		r = create_texture_2d(device, desc);

		if (!r)
		{
			return nullptr;
		}

		id = r->id;

		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			m_resources[id].first->create_shader_resource_view(device, srvDesc);
		}

		if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{
			m_resources[id].first->create_render_target_view(device, rtvDesc);
		}

		if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			m_resources[id].first->create_unordered_access_view(device, uavDesc);
		}

		if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			m_resources[id].first->create_depth_stencil_view(device, dsvDesc);
		}
	}
	else
	{
		id = r->id;
	}

	m_resources[id].second->usageFlag = true;

	return xng_new render_resource_handle(this, m_resources[id].second->id);
}

const render_resource * render_resource_manager::get_render_resource(uint32_t id) const
{
	return m_resources[id].first;
}

void render_resource_manager::clear(void)
{
	for (auto & r : m_resources)
	{
		xng_delete r.first;
		xng_delete r.second;
	}

	m_resources.clear();
	m_descriptionMap.clear();
}

void render_resource_manager::release(uint32_t id)
{
	m_resources[id].second->usageFlag = false;
}

render_resource_handle::render_resource_handle(void) :
	render_resource_handle(nullptr, 0) {}

render_resource_handle::render_resource_handle(render_resource_manager * manager, uint32_t id) :
	m_manager(manager), m_id(id) {}

render_resource_handle::~render_resource_handle(void)
{
	if (m_manager)
	{
		m_manager->release(m_id);
	}
}

const render_resource * render_resource_handle::get_render_resource(void) const
{
	return m_manager->get_render_resource(m_id);
}