#pragma once

#include <xng/gl/gl_headers.hpp>
#include <xng/res.hpp>
#include <xng/graphics/mesh.hpp>

namespace xng
{
	namespace gl
	{
		class gpu_mesh :
			public res::resource
		{

		public:

			gpu_mesh(void) = default;
			gpu_mesh(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader, res::resource_manager * owner);
			~gpu_mesh(void) override;

			void clear(void);

			XNG_INLINE GLuint get_positional_vao(void) const
			{
				return m_positionVAO;
			}

			XNG_INLINE GLuint get_non_positional_vao(void) const
			{
				return m_nonPositionalVAO;
			}

			XNG_INLINE GLuint get_indices_ebo(void) const
			{
				return m_indicesEBO;
			}

			XNG_INLINE GLenum get_indices_format(void) const
			{
				return m_indicesFormat;
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

			bool   load_impl(void) override;
			void   unload_impl(void) override;
			size_t calculate_size_impl(void) override;

		private:

			GLuint m_positionVAO;
			GLuint m_nonPositionalVAO;
			GLuint m_indicesEBO;

			GLuint m_positionVBO;
			GLenum m_indicesFormat;

			uint32_t m_numVertices;
			uint32_t m_numIndices;

		};

		typedef res::resource_ptr<gpu_mesh> gpu_mesh_ptr;

		gpu_mesh_ptr make_gpu_mesh(graphics::mesh_ptr mesh);

	}
}