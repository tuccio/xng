#include <xng/gl/shader.hpp>

#include <xng/core.hpp>

#include <fstream>
#include <iterator>

using namespace xng;
using namespace xng::gl;

enum { XNG_GL_SHADER_ERROR_UNCOMPILED = -256, XNG_GL_SHADER_ERROR_FILE_ACCESS, XNG_GL_SHADER_ERROR_COMPILATION, XNG_GL_SHADER_ERROR_CREATION };

shader::shader(void) : m_shader(0), m_error(XNG_GL_SHADER_ERROR_UNCOMPILED) {}

shader::shader(shader && rhs)
{
	m_shader     = rhs.m_shader;
	rhs.m_shader = 0;
	rhs.m_error  = XNG_GL_SHADER_ERROR_UNCOMPILED;
}

shader::~shader(void)
{
	clear();
}


void shader::clear(void)
{
	if (m_shader)
	{
		glDeleteShader(m_shader);
		m_shader = 0;
		m_error  = XNG_GL_SHADER_ERROR_UNCOMPILED;
	}
}

bool shader::compile_from_file(xng_gl_shader_type type, const char * filename, const shader_macro * macros)
{
	std::ifstream f(filename);

	if (f)
	{
		f >> std::noskipws;
		std::string source = std::string(std::istream_iterator<char>(f), std::istream_iterator<char>());
		
		if (compile(type, source.c_str(), macros))
		{
			return true;
		}
	}
	else
	{
		m_error = XNG_GL_SHADER_ERROR_FILE_ACCESS;
	}

	XNG_LOG("xng::gl::shader::compile_from_file", XNG_LOG_STREAM() << "Error occurred while compiling \"" << filename << "\"." << std::endl << get_compilation_error());
	return false;
}

bool shader::compile_from_string(xng_gl_shader_type type, const char * str, const shader_macro * macros)
{
	if (compile(type, str, macros))
	{
		return true;
	}

	XNG_LOG("xng::gl::shader::compile_from_string", XNG_LOG_STREAM() << "Compilation error." << std::endl << get_compilation_error());
	return false;
}

bool shader::compile(xng_gl_shader_type type, const char * str, const shader_macro * macros)
{
	if (create_shader(type))
	{
		glShaderSource(m_shader, 1, &str, nullptr);
		glCompileShader(m_shader);

		GLint success;
		glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);

		if (success != GL_FALSE)
		{
			m_type = type;
			return true;
		}
		else
		{
			m_error = XNG_GL_SHADER_ERROR_COMPILATION;
			return false;
		}
	}
	else
	{
		m_error = XNG_GL_SHADER_ERROR_CREATION;
		return false;
	}
}

std::string shader::get_compilation_error(void) const
{
	if (*this)
	{
		return "Compilation succeeded.";
	}
	else if (m_error == XNG_GL_SHADER_ERROR_UNCOMPILED)
	{
		return "Shader is yet to be compiled, no errors to signal.";
	}
	else if (m_error == XNG_GL_SHADER_ERROR_FILE_ACCESS)
	{
		return "Can't access the shader source file.";
	}
	else if (m_error == XNG_GL_SHADER_ERROR_CREATION)
	{
		return "Failure on glCreateShader call, cannot create the shader.";
	}
	else if (m_shader && m_error == XNG_GL_SHADER_ERROR_COMPILATION)
	{
		GLint length;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &length);

		std::string errorLog(length, '\0');
		glGetShaderInfoLog(m_shader, length, &length, &errorLog[0]);

		return errorLog;
	}
	else
	{
		return "Unknown error.";
	}
}

GLuint shader::get(void) const
{
	return m_shader;
}

int shader::get_shader_type(void) const
{
	return m_type;
}

shader::operator bool() const
{
	return m_shader != 0 && m_type >= 0;
}

bool shader::create_shader(xng_gl_shader_type type)
{
	clear();

	if (!m_shader)
	{
		switch (type)
		{
		case XNG_GL_SHADER_VERTEX:
			m_shader = glCreateShader(GL_VERTEX_SHADER);
			break;

		case XNG_GL_SHADER_FRAGMENT:
			m_shader = glCreateShader(GL_FRAGMENT_SHADER);
			break;

		default:
			break;
		}
	}

	return m_shader != 0;
}