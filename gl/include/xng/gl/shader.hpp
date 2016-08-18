#pragma once

#include <xng/gl/gl_headers.hpp>
#include <string>

enum xng_gl_shader_type
{
	XNG_GL_SHADER_VERTEX,
	XNG_GL_SHADER_FRAGMENT
};

#define XNG_GL_SHADER_FILE(File) (XNG_GL_SHADER_FOLDER ## File)

namespace xng
{
	namespace gl
	{
		struct shader_macro
		{
			const char * name;
			const char * definition;
		};

		class shader
		{

		public:

			shader(void);
			shader(const shader &) = delete;
			shader(shader &&);

			~shader(void);

			void clear(void);

			bool compile_from_file(xng_gl_shader_type type, const char * filename, const shader_macro * macros = nullptr);
			bool compile_from_string(xng_gl_shader_type type, const char * str, const shader_macro * macros = nullptr);
			std::string get_compilation_error(void) const;

			GLuint get(void) const;
			int get_shader_type(void) const;

			operator bool() const;

		private:

			bool create_shader(xng_gl_shader_type type);
			bool compile(xng_gl_shader_type type, const char * str, const shader_macro * macros = nullptr);

			GLuint m_shader;

			union
			{
				int m_type;
				int m_error;
			};

		};
	}
}