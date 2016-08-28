#pragma once

#include <xng/gl/shader.hpp>
#include <xng/math.hpp>

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
			void detach_shader(const shader * s);
			
			bool link(void);
			void use(void) const;
			void dispose(void) const;

			bool is_linked(void) const;

			operator bool() const;

			std::string get_linking_error(void) const;

			GLint get_uniform_location(const char * uniform) const;

			void set_uniform(GLint location, GLfloat x);
			void set_uniform(GLint location, const math::float2 & x);
			void set_uniform(GLint location, const math::float3 & x);
			void set_uniform(GLint location, const math::float4 & x);

			void set_uniform(GLint location, GLdouble x);
			void set_uniform(GLint location, const math::double2 & x);
			void set_uniform(GLint location, const math::double3 & x);
			void set_uniform(GLint location, const math::double4 & x);

			void set_uniform(GLint location, GLint x);
			void set_uniform(GLint location, const math::int2 & x);
			void set_uniform(GLint location, const math::int3 & x);
			void set_uniform(GLint location, const math::int4 & x);

			void set_uniform(GLint location, GLuint x);
			void set_uniform(GLint location, const math::uint2 & x);
			void set_uniform(GLint location, const math::uint3 & x);
			void set_uniform(GLint location, const math::uint4 & x);

			void set_uniform(GLint location, const math::float2x2 & x);
			void set_uniform(GLint location, const math::float3x3 & x);
			void set_uniform(GLint location, const math::float4x4 & x);

		private:

			bool create_program(void);

			GLuint m_program;
			bool   m_linked;

		};
	}
}