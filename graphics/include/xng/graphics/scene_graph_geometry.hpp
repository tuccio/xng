#include <xng/graphics/scene_graph_node.hpp>
#include <xng/graphics/mesh.hpp>
#include <xng/graphics/material.hpp>

namespace xng
{
    namespace graphics
    {
        class scene_graph_geometry :
            public scene_graph_node
        {

        public:
            
            scene_graph_geometry(scene_graph_node * parent);
            scene_graph_geometry(const scene_graph_geometry &) = default;

            mesh_ptr get_mesh(void) const;
            void set_mesh(mesh_ptr mesh);

            material_ptr get_material(void) const;
            void set_material(material_ptr material);

            bool is_static(void) const;
            void set_static(bool s);

            math::sphere get_sphere(void);

        protected:

            void update_impl(void) override;

        private:

            mesh_ptr     m_mesh;
            material_ptr m_material;
            bool         m_static;

        };
    }
}