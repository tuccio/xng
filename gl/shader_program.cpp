#include <xng/gl/shader_program.hpp>
#include <xng/core.hpp>

#include <glm/gtc/type_ptr.hpp>

using namespace xng;
using namespace xng::gl;

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
		glDeleteProgram(m_program);
		m_program = 0;
		m_linked  = false;
	}
}

bool shader_program::attach_shader(const shader * s)
{
	if ((m_program || create_program()) && s && *s)
	{
		glAttachShader(m_program, s->get());
		return true;
	}

	return false;
}

bool shader_program::link(void)
{
	m_linked = false;

	if (m_program)
	{
		glLinkProgram(m_program);

		GLint success;
		glGetProgramiv(m_program, GL_LINK_STATUS, &success);

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
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

		std::string errorLog(length, '\0');
		glGetProgramInfoLog(m_program, length, &length, &errorLog[0]);

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
	glUseProgram(m_program);
}

void shader_program::dispose(void) const
{
	glUseProgram(0);
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
	return glGetUniformLocation(m_program, uniform);
}

/* float */

void shader_program::set_uniform(GLint location, GLfloat x)
{
	glUniform1f(location, x);
}

void shader_program::set_uniform(GLint location, const glm::vec2 & x)
{
	glUniform2fv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::vec3 & x)
{
	glUniform3fv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::vec4 & x)
{
	glUniform4fv(location, 1, glm::value_ptr(x));
}

/* double */

void shader_program::set_uniform(GLint location, GLdouble x)
{
	glUniform1d(location, x);
}

void shader_program::set_uniform(GLint location, const glm::dvec2 & x)
{
	glUniform2dv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::dvec3 & x)
{
	glUniform3dv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::dvec4 & x)
{
	glUniform4dv(location, 1, glm::value_ptr(x));
}

/* int */

void shader_program::set_uniform(GLint location, GLint x)
{
	glUniform1i(location, x);
}

void shader_program::set_uniform(GLint location, const glm::ivec2 & x)
{
	glUniform2iv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::ivec3 & x)
{
	glUniform3iv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::ivec4 & x)
{
	glUniform4iv(location, 1, glm::value_ptr(x));
}


/* unsigned int */

void shader_program::set_uniform(GLint location, GLuint x)
{
	glUniform1ui(location, x);
}

void shader_program::set_uniform(GLint location, const glm::uvec2 & x)
{
	glUniform2uiv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::uvec3 & x)
{
	glUniform3uiv(location, 1, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::uvec4 & x)
{
	glUniform4uiv(location, 1, glm::value_ptr(x));
}

/* matrices */

void shader_program::set_uniform(GLint location, const glm::mat2 & x)
{
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::mat3 & x)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(x));
}

void shader_program::set_uniform(GLint location, const glm::mat4 & x)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(x));
}