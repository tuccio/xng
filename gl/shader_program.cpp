#include <xng/gl/shader_program.hpp>
#include <xng/core.hpp>
#include <xng/gl/errors.hpp>

using namespace xng;
using namespace xng::gl;
using namespace xng::math;

shader_program::shader_program(void) : m_program(0), m_linked(false) {}

shader_program::shader_program(shader_program && rhs)
{
	m_program     = rhs.m_program;
	rhs.m_program = 0;
}

shader_program::~shader_program(void)
{
	clear();
}

void shader_program::clear(void)
{
	if (m_program)
	{
		XNG_GL_CHECK(glDeleteProgram(m_program));
		m_program = 0;
		m_linked  = false;
	}
}

bool shader_program::attach_shader(const shader * s)
{
	if ((m_program || create_program()) && s && *s)
	{
		XNG_GL_CHECK(glAttachShader(m_program, s->get()));
		return true;
	}

	return false;
}

void shader_program::detach_shader(const shader * s)
{
	XNG_GL_CHECK(glDetachShader(m_program, s->get()));
}

bool shader_program::link(void)
{
	m_linked = false;

	if (m_program)
	{
		XNG_GL_CHECK(glLinkProgram(m_program));

		GLint success;
		XNG_GL_CHECK(glGetProgramiv(m_program, GL_LINK_STATUS, &success));

		m_linked = success != GL_FALSE;
	}

	if (!m_linked)
	{
		XNG_LOG("xng::gl::shader_program::link", XNG_LOG_STREAM() << "Error occurred while linking." << std::endl << get_linking_error());
	}
	
	return m_linked;
}

std::string shader_program::get_linking_error(void) const
{
	if (m_program)
	{
		GLint length;
		XNG_GL_CHECK(glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length));

		std::string errorLog(length, '\0');
		XNG_GL_CHECK(glGetProgramInfoLog(m_program, length, &length, &errorLog[0]));

		return errorLog;
	}
	else
	{
		return std::string();
	}
}

void shader_program::use(void) const
{
	assert(m_linked && "Trying to use an unlinked program.");
	XNG_GL_CHECK(glUseProgram(m_program));
}

void shader_program::dispose(void) const
{
	XNG_GL_CHECK(glUseProgram(0));
}

bool shader_program::create_program(void)
{
	clear();
	m_program = glCreateProgram();
	return m_program != 0;
}

shader_program::operator bool() const
{
	return m_program != 0 && m_linked;
}

/* Uniforms */

GLint shader_program::get_uniform_location(const char * uniform) const
{
	return XNG_GL_RETURN_CHECK(glGetUniformLocation(m_program, uniform));
}

/* float */

void shader_program::set_uniform(GLint location, GLfloat x)
{
	XNG_GL_CHECK(glUniform1f(location, x));
}

void shader_program::set_uniform(GLint location, const float2 & x)
{
	XNG_GL_CHECK(glUniform2fv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const float3 & x)
{
	XNG_GL_CHECK(glUniform3fv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const float4 & x)
{
	XNG_GL_CHECK(glUniform4fv(location, 1, x.data()));
}

/* double */

void shader_program::set_uniform(GLint location, GLdouble x)
{
	XNG_GL_CHECK(glUniform1d(location, x));
}

void shader_program::set_uniform(GLint location, const double2 & x)
{
	XNG_GL_CHECK(glUniform2dv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const double3 & x)
{
	XNG_GL_CHECK(glUniform3dv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const double4 & x)
{
	XNG_GL_CHECK(glUniform4dv(location, 1, x.data()));
}

/* int */

void shader_program::set_uniform(GLint location, GLint x)
{
	XNG_GL_CHECK(glUniform1i(location, x));
}

void shader_program::set_uniform(GLint location, const int2 & x)
{
	XNG_GL_CHECK(glUniform2iv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const int3 & x)
{
	XNG_GL_CHECK(glUniform3iv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const int4 & x)
{
	XNG_GL_CHECK(glUniform4iv(location, 1, x.data()));
}


/* unsigned int */

void shader_program::set_uniform(GLint location, GLuint x)
{
	XNG_GL_CHECK(glUniform1ui(location, x));
}

void shader_program::set_uniform(GLint location, const uint2 & x)
{
	XNG_GL_CHECK(glUniform2uiv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const uint3 & x)
{
	XNG_GL_CHECK(glUniform3uiv(location, 1, x.data()));
}

void shader_program::set_uniform(GLint location, const uint4 & x)
{
	XNG_GL_CHECK(glUniform4uiv(location, 1, x.data()));
}

/* matrices */

void shader_program::set_uniform(GLint location, const float2x2 & x)
{
	XNG_GL_CHECK(glUniformMatrix2fv(location, 1, GL_FALSE, x.data()));
}

void shader_program::set_uniform(GLint location, const float3x3 & x)
{
	XNG_GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, x.data()));
}

void shader_program::set_uniform(GLint location, const float4x4 & x)
{
	XNG_GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, x.data()));
}