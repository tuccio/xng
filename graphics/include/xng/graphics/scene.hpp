#pragma once

#include <xng/graphics/scene_graph.hpp>
#include <xng/graphics/camera.hpp>

#include <xng/core.hpp>

#include <vector>

namespace xng
{
    namespace graphics
    {
        class extracted_scene;

        class scene :
            public core::refcounted_obj<scene>
        {

        public:

            typedef std::vector<scene_graph_geometry*> geometry_vector;
            typedef std::vector<scene_graph_camera*>   camera_vector;
            typedef std::vector<scene_graph_light*>    light_vector;

            virtual ~scene(void) = default;

            virtual scene_graph * get_scene_graph(void) = 0;

            virtual geometry_vector get_frustum_culling_static(const math::frustum & f) = 0;
            virtual geometry_vector get_frustum_culling_dynamic(const math::frustum & f) = 0;

            virtual geometry_vector get_geometry_nodes(void) = 0;
            virtual camera_vector   get_camera_nodes(void) = 0;
            virtual light_vector    get_light_nodes(void) = 0;

            virtual scene_graph_camera * get_active_camera(void) = 0;
            virtual void set_active_camera(scene_graph_camera * camera) = 0;

            virtual void update(void) = 0;

            extracted_scene extract(void);

        };

        typedef core::refcounted_ptr<scene> scene_ptr;

        struct extracted_renderable
        {
            uint32_t       id;
            mesh_ptr       mesh;
            material_ptr   material;
            math::float4x4 world;
        };

        typedef std::vector<uint32_t>             renderable_index_vector;
        typedef std::vector<extracted_renderable> renderable_vector;

        struct extracted_light
        {
            uint32_t       id;
            xng_light_type type;
            math::float3   position;
            math::float3   direction;
            math::float3   luminance;
            math::float3   ambient;
            float          cutoff;
            bool           shadowCasting;
        };

        typedef std::vector<uint32_t>                light_index_vector;
        typedef std::vector<extracted_light>         light_vector;
        typedef std::vector<renderable_index_vector> light_lit_vector;

        class extracted_scene
        {

        public:

            extracted_scene(void) = default;
            extracted_scene(const extracted_scene &) = default;
            extracted_scene(extracted_scene &&) = default;

            extracted_scene & operator= (const extracted_scene &) = default;
            extracted_scene & operator= (extracted_scene &&) = default;

            void clear(void);

            const camera * get_active_camera(void) const;

            const renderable_vector & get_renderables(void) const;

            const renderable_index_vector & get_frustum_culling_static(void) const;
            const renderable_index_vector & get_frustum_culling_dynamic(void) const;

            const light_vector & get_lights(void) const;

            const renderable_index_vector & get_shadow_casting_objects(uint32_t index) const;

        private:

            camera m_activeCamera;

            renderable_vector       m_renderables;
            renderable_index_vector m_visibleDynamicObjects;
            renderable_index_vector m_visibleStaticObjects;

            light_vector            m_lights;
            light_lit_vector        m_shadowCastingObjects;

            friend class scene;
        };
    }
}