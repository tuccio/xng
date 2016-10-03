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
		auto frustumCullingDynamic = get_frustum_culling_dynamic(cam->get_camera());
		auto frustumCullingStatic  = get_frustum_culling_static(cam->get_camera());

		auto lights = get_light_nodes();;

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

				xscene.m_renderables[index].id       = g->get_id();
				xscene.m_renderables[index].mesh     = g->get_mesh();
				xscene.m_renderables[index].material = g->get_material();
				xscene.m_renderables[index].world    = g->get_global_matrix();
			}
			else
			{
				index = it->second;
			}

			v.push_back(index);
		};

		xscene.m_activeCamera = *cam->get_camera();

		std::for_each(frustumCullingDynamic.begin(), frustumCullingDynamic.end(), std::bind(add_geometry, std::ref(xscene.m_visibleDynamicObjects), std::placeholders::_1));
		std::for_each(frustumCullingStatic.begin(), frustumCullingStatic.end(), std::bind(add_geometry, std::ref(xscene.m_visibleStaticObjects), std::placeholders::_1));
		
		xscene.m_lights.reserve(lights.size());

		std::transform(lights.begin(), lights.end(), std::back_inserter(xscene.m_lights),
			[](scene_graph_light * l)
		{
			extracted_light extractedLight;

			extractedLight.id            = l->get_id();
			extractedLight.ambient       = l->get_ambient();
			extractedLight.luminance     = l->get_color() * l->get_intensity();
			extractedLight.direction     = l->get_global_rotation() * l->get_direction();
			extractedLight.position      = l->get_position();
			extractedLight.cutoff        = l->get_cutoff_angle();
			extractedLight.shadowCasting = l->is_shadow_caster();
			extractedLight.type          = l->get_light_type();

			return extractedLight;
		});
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

const renderable_vector & extracted_scene::get_renderables(void) const
{
	return m_renderables;
}

const renderable_index_vector & extracted_scene::get_frustum_culling_static(void) const
{
	return m_visibleStaticObjects;
}

const renderable_index_vector & extracted_scene::get_frustum_culling_dynamic(void) const
{
	return m_visibleDynamicObjects;
}

const light_vector & extracted_scene::get_lights(void) const
{
	return m_lights;
}

const renderable_index_vector & extracted_scene::get_shadow_casting_objects(uint32_t index) const
{
	return m_shadowCastingObjects[index];
}