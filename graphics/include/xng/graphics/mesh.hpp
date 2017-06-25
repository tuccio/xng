#pragma once

#include <xng/math.hpp>
#include <xng/geometry.hpp>
#include <xng/res.hpp>

#include <cstdint>
#include <memory>
#include <vector>

#define XNG_MESH_MAX_TEXCOORDS 8

enum mesh_storage_semantic
{
    XNG_MESH_STORAGE_NONE         = 0,
    XNG_MESH_STORAGE_NORMALS      = 1,
    XNG_MESH_STORAGE_TANGENTS     = 2,
    XNG_MESH_STORAGE_BITANGENTS   = 4,
    XNG_MESH_STORAGE_TANGENTSPACE = 6,
    XNG_MESH_STORAGE_TEXCOORDS0   = 8,
    XNG_MESH_STORAGE_TEXCOORDS1   = 16,
    XNG_MESH_STORAGE_TEXCOORDS2   = 32,
    XNG_MESH_STORAGE_TEXCOORDS3   = 64,
    XNG_MESH_STORAGE_TEXCOORDS4   = 128,
    XNG_MESH_STORAGE_TEXCOORDS5   = 256,
    XNG_MESH_STORAGE_TEXCOORDS6   = 1024,
    XNG_MESH_STORAGE_TEXCOORDS7   = 2048,
};

namespace xng
{
    namespace graphics
    {
        class mesh :
            public res::resource
        {

        public:

            static const char * resource_type;

            mesh(void) = default;
            mesh(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
            ~mesh(void) override;

            bool create(size_t vertices, size_t triangles, uint32_t storage);
            void clear(void);

            bool add_storage_semantic(mesh_storage_semantic semantic);
            bool remove_storage_semantic(mesh_storage_semantic semantic);

            bool calculate_tangent_space(void);

            XNG_INLINE uint32_t get_num_triangles(void) const { return m_numTriangles; }
            XNG_INLINE uint32_t get_num_vertices(void) const { return m_numVertices; }
            XNG_INLINE uint32_t get_num_indices(void) const { return 3 * m_numTriangles; }

            XNG_INLINE bool     has_storage_semantic(mesh_storage_semantic semantic) const { return (semantic & m_storageFlags) != 0; }
            XNG_INLINE uint32_t get_storage_semantic_flags(void) const { return m_storageFlags; }

            uint32_t get_parameters_storage_semantic_flags(void);

            XNG_INLINE float * get_vertices(void) { return &m_vertices[0].x; }
            XNG_INLINE const float * get_vertices(void) const { return &m_vertices[0].x; }

            XNG_INLINE float * get_normals(void) { return &m_normals[0].x; }
            XNG_INLINE const float * get_normals(void) const { return &m_normals[0].x; }

            XNG_INLINE float * get_tangents(void) { return &m_tangents[0].x; }
            XNG_INLINE const float * get_tangents(void) const { return &m_tangents[0].x; }

            XNG_INLINE float * get_bitangents(void) { return &m_bitangents[0].x; }
            XNG_INLINE const float * get_bitangents(void) const { return &m_bitangents[0].x; }

            XNG_INLINE float * get_texcoords(int i) { return &m_texcoords[i][0].x; }
            XNG_INLINE const float * get_texcoords(int i) const { return &m_texcoords[i][0].x; }

            XNG_INLINE uint32_t * get_indices(void) { return &m_indices[0].x; }
            XNG_INLINE const uint32_t * get_indices(void) const { return &m_indices[0].x; }

            std::vector<uint8_t> pack_interleaved_data(bool packPositions, uint32_t flags) const;
            static size_t calculate_stride(uint32_t flags);

            const math::aabb & get_aabb(void) const;
            const math::sphere & get_bounding_sphere(void) const;

            void compute_aabb(void);
            void compute_bounding_sphere(void);

        protected:

            bool   load_impl(const void * userdata) override;
            void   unload_impl(void) override;
            size_t calculate_size_impl(void) override;

        private:


        private:

            std::vector<math::float3> m_vertices;
            std::vector<math::float3> m_normals;
            std::vector<math::float3> m_tangents;
            std::vector<math::float3> m_bitangents;
            std::vector<math::float2> m_texcoords[XNG_MESH_MAX_TEXCOORDS];
            std::vector<math::uint3>  m_indices;

            math::aabb   m_aabb;
            math::sphere m_sphere;

            uint32_t   m_numVertices;
            uint32_t   m_numTriangles;

            uint32_t   m_storageFlags;

            // Internal

            size_t calculate_size_internal(void) const;

        };

        typedef res::resource_ptr<mesh>           mesh_ptr;
        typedef res::basic_resource_manager<mesh> mesh_manager;

        class box_mesh_loader :
            res::resource_loader
        {

        public:

            bool load(res::resource * r, const void * userdata = nullptr) override;
            void unload(res::resource * r) override;

        };
    }
}