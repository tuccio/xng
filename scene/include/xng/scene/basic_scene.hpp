#include <xng/graphics.hpp>

namespace xng
{
    namespace scene
    {
        class basic_scene :
            public graphics::scene
        {

        public:

            basic_scene(void);
            basic_scene(const basic_scene &) = default;

            graphics::scene_graph * get_scene_graph(void) override;

            geometry_vector get_frustum_culling_static(const math::frustum & f) override;
            geometry_vector get_frustum_culling_dynamic(const math::frustum & f) override;

            geometry_vector get_geometry_nodes(void) override;
            camera_vector get_camera_nodes(void) override;
            light_vector get_light_nodes(void) override;

            graphics::scene_graph_camera * get_active_camera(void) override;
            void set_active_camera(graphics::scene_graph_camera * camera) override;

            void update(void) override;

        private:

            graphics::scene_graph m_sceneGraph;
            graphics::scene_graph_camera * m_activeCamera;

        };
    }
}