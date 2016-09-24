#include <xng/graphics/scene.hpp>

#include <unordered_set>
#include <algorithm>
#include <functional>

using namespace xng::graphics;

extracted_scene scene::extract(void)
{
	extracted_scene xscene;

	auto cam = get_active_camera();

	if (cam)
	{
		auto fcd = frustum_culling_dynamic(cam->get_camera());
		auto fcs = frustum_culling_static(cam->get_camera());

		std::unordered_map<scene_graph_geometry*, uint32_t> geometryIndex;

		auto add_geometry =
			[&](std::vector<uint32_t> & v, scene_graph_geometry * g)
		{
			uint32_t index;

			auto it = geometryIndex.find(g);

			if (it == geometryIndex.end())
			{
				index = xscene.m_renderables.size();
				xscene.m_renderables.emplace_back();

				geometryIndex[g] = index;

				xscene.m_renderables[index].mesh  = g->get_mesh();
				xscene.m_renderables[index].world = g->get_global_matrix();
			}
			else
			{
				index = it->second;
			}

			v.push_back(index);
		};

		xscene.m_activeCamera = *cam->get_camera();

		std::for_each(fcd.begin(), fcd.end(), std::bind(add_geometry, std::ref(xscene.m_visibleDynamicObjects), std::placeholders::_1));
		std::for_each(fcs.begin(), fcs.end(), std::bind(add_geometry, std::ref(xscene.m_visibleStaticObjects), std::placeholders::_1));
	}

	return xscene;
}

void extracted_scene::clear(void)
{
	m_renderables.clear();
	m_visibleDynamicObjects.clear();
	m_visibleStaticObjects.clear();
}

const camera * extracted_scene::get_active_camera(void) const
{
	return &m_activeCamera;
}

const extracted_renderable * extracted_scene::get_renderable(uint32_t index) const
{
	return &m_renderables[index];
}

const extracted_scene::renderable_index_vector & extracted_scene::frustum_culling_static(void) const
{
	return m_visibleStaticObjects;
}

const extracted_scene::renderable_index_vector & extracted_scene::frustum_culling_dynamic(void) const
{
	return m_visibleDynamicObjects;
}