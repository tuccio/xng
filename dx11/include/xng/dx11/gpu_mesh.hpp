#pragma once

#include <xng/dx11/dx11_headers.hpp>
#include <xng/res.hpp>
#include <xng/graphics/mesh.hpp>
#include <tuple>

namespace xng
{
    namespace dx11
    {
        class gpu_mesh :
            public res::resource
        {

        public:

            struct load_data
            {
                ID3D11Device * device;
            };

            static const char * resource_type;

            gpu_mesh(void) = default;
            gpu_mesh(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
            ~gpu_mesh(void) override;

            void clear(void);

            XNG_INLINE ID3D11Buffer * get_positional_data(void) const
            {
                return m_position.get();
            }

            XNG_INLINE ID3D11Buffer * get_non_positional_data(void) const
            {
                return m_nonPositionalData.get();
            }

            XNG_INLINE ID3D11Buffer * get_indices_data(void) const
            {
                return m_indices.get();
            }

            XNG_INLINE uint32_t get_positional_data_stride(void) const
            {
                return m_positionStride;
            }

            XNG_INLINE uint32_t get_positional_data_offset(void) const
            {
                return m_positionOffset;
            }

            XNG_INLINE uint32_t get_non_positional_data_stride(void) const
            {
                return m_nonPositionalDataStride;
            }

            XNG_INLINE uint32_t get_non_positional_data_offset(void) const
            {
                return m_nonPositionalDataOffset;
            }

            XNG_INLINE DXGI_FORMAT get_indices_format(void) const
            {
                return m_indicesFormat;
            }

            XNG_INLINE uint32_t get_indices_offset(void) const
            {
                return m_indicesOffset;
            }

            XNG_INLINE uint32_t get_num_vertices(void) const
            {
                return m_numVertices;
            }

            XNG_INLINE uint32_t get_num_indices(void) const
            {
                return m_numIndices;
            }

        protected:

            bool   load_impl(const void * userdata) override;
            void   unload_impl(void) override;
            size_t calculate_size_impl(void) override;

        private:

            com_ptr<ID3D11Buffer> m_position;
            com_ptr<ID3D11Buffer> m_nonPositionalData;
            com_ptr<ID3D11Buffer> m_indices;

            uint32_t m_positionStride;
            uint32_t m_positionOffset;

            uint32_t m_nonPositionalDataStride;
            uint32_t m_nonPositionalDataOffset;

            DXGI_FORMAT m_indicesFormat;
            uint32_t    m_indicesOffset;

            uint32_t m_numVertices;
            uint32_t m_numIndices;

        };

        typedef res::resource_ptr<gpu_mesh>           gpu_mesh_ptr;
        typedef res::basic_resource_manager<gpu_mesh> gpu_mesh_manager;
    }
}