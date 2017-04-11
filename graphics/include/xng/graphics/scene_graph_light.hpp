#pragma once

#include <xng/graphics/scene_graph_node.hpp>

enum xng_light_type
{
    XNG_LIGHT_DIRECTIONAL, XNG_LIGHT_SPOT, XNG_LIGHT_POINT
};

namespace xng
{
    namespace graphics
    {
        class scene_graph_light :
            public scene_graph_node
        {

        public:

            scene_graph_light(scene_graph_node * parent);
            scene_graph_light(const scene_graph_light &) = default;

            bool is_static(void) const;
            void set_static(bool s);

            bool is_shadow_caster(void) const;
            void set_shadow_caster(bool shadowCaster);

            math::float3 get_position(void);
            void set_position(const math::float3 & position);

            math::float3 get_direction(void);
            void set_direction(const math::float3 & direction);

            xng_light_type get_light_type(void) const;
            void set_light_type(xng_light_type type);

            const math::ubyte3 & get_color(void) const;
            void set_color(const math::ubyte3 & color);

            float get_intensity(void) const;
            void set_intensity(float intensity);

            math::float3 get_luminance(void) const;

            const math::ubyte3 & get_ambient_color(void) const;
            void set_ambient_color(const math::ubyte3 & ambient);

            math::float3 get_ambient_luminance(void) const;

            float get_ambient_intensity(void) const;
            void set_ambient_intensity(float intensity);

            float get_cutoff_angle(void) const;
            void set_cutoff_angle(float angle);

        private:

            bool           m_shadowCaster;
            bool           m_static;
            xng_light_type m_type;

            math::ubyte3   m_color;
            math::ubyte3   m_ambient;
            float          m_intensity;
            float          m_ambientIntensity;
            float          m_cutoff;

        };
    }
}