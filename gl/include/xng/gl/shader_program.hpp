#pragma once

#include <xng/gl/shader.hpp>
#include <glm/glm.hpp>

namespace xng
{
	namespace gl
	{
		class shader_program
		{

		public:

			shader_program(void);
			shader_program(const shader_program &) = delete;
			shader_program(shader_program &&);

			~shader_program(void);

			void clear(void);

			bool attach_shader(const shader * s);
			
			bool link(void);
			void use(void) const;
			void dispose(void) const;

			operator bool() const;

			std::string get_linking_error(void) const;

			GLint get_uniform_location(const char * uniform) const;

			void set_uniform(GLint location, GLfloat x);
			void set_uniform(GLint location, const glm::vec2 & x);
			void set_uniform(GLint location, const glm::vec3 & x);
			void set_uniform(GLint location, const glm::vec4 & x);

			void set_uniform(GLint location, GLdouble x);
			void set_uniform(GLint location, const glm::dvec2 & x);
			void set_uniform(GLint location, const glm::dvec3 & x);
			void set_uniform(GLint location, const glm::dvec4 & x);

			void set_uniform(GLint location, GLint x);
			void set_uniform(GLint location, const glm::ivec2 & x);
			void set_uniform(GLint location, const glm::ivec3 & x);
			void set_uniform(GLint location, const glm::ivec4 & x);

			void set_uniform(GLint location, GLuint x);
			void set_uniform(GLint location, const glm::uvec2 & x);
			void set_uniform(GLint location, const glm::uvec3 & x);
			void set_uniform(GLint location, const glm::uvec4 & x);

			void set_uniform(GLint location, const glm::mat2 & x);
			void set_uniform(GLint location, const glm::mat3 & x);
			void set_uniform(GLint location, const glm::mat4 & x);

		private:

			bool create_program(void);

			GLuint m_program;
			bool   m_linked;

		};
	}
}