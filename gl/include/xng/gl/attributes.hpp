#pragma once

#include <xng/gl/gl_headers.hpp>
#include <xng/gl/errors.hpp>

#include <xng/graphics/mesh.hpp>
#include <vector>

#define XNG_GL_ATTRIBUTE_POSITION  0
#define XNG_GL_ATTRIBUTE_NORMAL    1
#define XNG_GL_ATTRIBUTE_TANGENT   2
#define XNG_GL_ATTRIBUTE_BITANGENT 3

#define XNG_GL_ATTRIBUTE_MAX       16
#define XNG_GL_ATTRIBUTE_SENTINEL  ((GLuint)-1)

namespace xng
{
	namespace gl
	{

		class attributes_enabler
		{

		public:

			attributes_enabler(const graphics::mesh_ptr & m)
			{
				create_attributes_vector(m->get_storage_semantic_flags());
				enable_attributes();
			}

			attributes_enabler(uint32_t flags)
			{
				create_attributes_vector(flags);
				enable_attributes();
			}

			~attributes_enabler(void)
			{
				disable_attributes();
			}
			
		private:

			void create_attributes_vector(uint32_t flags)
			{
				uint32_t i = 0;

				m_attributes[i++] = XNG_GL_ATTRIBUTE_POSITION;
				
				if (flags & XNG_MESH_STORAGE_NORMALS)
				{
					m_attributes[i++] = XNG_GL_ATTRIBUTE_NORMAL;
				}

				while (i < XNG_GL_ATTRIBUTE_MAX) m_attributes[i++] = XNG_GL_ATTRIBUTE_SENTINEL;
			}

			void enable_attributes(void)
			{
				for (int i = 0; m_attributes[i] != XNG_GL_ATTRIBUTE_SENTINEL; ++i)
				{
					XNG_GL_CHECK(glEnableVertexAttribArray(m_attributes[i]));
				}
			}

			void disable_attributes(void)
			{
				for (int i = 0; m_attributes[i] != XNG_GL_ATTRIBUTE_SENTINEL; ++i)
				{
					XNG_GL_CHECK(glDisableVertexAttribArray(m_attributes[i]));
				}
			}

			GLuint m_attributes[XNG_GL_ATTRIBUTE_MAX];

		};
	}
}