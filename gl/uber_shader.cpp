#include <xng/gl/uber_shader.hpp>
#include <xng/core.hpp>

using namespace xng::gl;
using namespace xng::graphics;

uber_shader::uber_shader(void) :
	m_preprocessed(false) {}

bool uber_shader::preprocess(const char * filename)
{
	if (is_preprocessed())
	{
		return false;
	}

	clear();

	m_filename = filename;
	std::vector<std::string> errors;

	shader_preprocessor pp;

	struct gl_line_fixer :
		shader_preprocessor_observer
	{
		void on_include_begin(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) override
		{
			os << "#line " << line << std::endl;
		}

		void on_include_end(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) override
		{
			os << "#line " << line << std::endl;
		}
	} lineFixer;

	pp.add_observer(&lineFixer);

	m_preprocessed = pp.preprocess(filename, &m_preprocessedShader, &errors);

	if (!m_preprocessed)
	{
		auto stream = XNG_LOG_STREAM();
		stream << "A list of errors was raised when preprocessing uber shader \"" << filename << "\":" << std::endl;
		for (auto & e : errors) stream << e << std::endl;
		XNG_LOG("xng::gl::uber_shader", stream);
	}

	return m_preprocessed;
}

bool uber_shader::is_preprocessed(void) const
{
	return m_preprocessed;
}

void uber_shader::clear(void)
{
	m_preprocessed = false;
	m_shaders.clear();
	m_filename.clear();
	m_preprocessedShader = decltype(m_preprocessedShader){};
}

const shader_program * uber_shader::compile(const char * name, std::initializer_list<shader_macro> macros)
{
	auto shaderIt = m_shaders.find(name);

	if (shaderIt != m_shaders.end())
	{
		return shaderIt->second.get();
	}

	auto it = m_shaders.emplace(name, std::unique_ptr<shader_program>(new shader_program()));
	auto & program = it.first->second;

	std::vector<shader> shaders;
	std::vector<shader_macro> macrosVector = macros;
	macrosVector.push_back(shader_macro{ nullptr, nullptr });

	if (m_preprocessedShader.has_section("vertex"))
	{
		shaders.emplace_back();

		if (!shaders.back().compile_from_string(
			XNG_GL_SHADER_VERTEX,
			m_preprocessedShader.get_section("vertex")->content.c_str(),
			&macrosVector[0]))
		{
			shaders.pop_back();
		}
	}

	if (m_preprocessedShader.has_section("fragment"))
	{
		shaders.emplace_back();

		if (!shaders.back().compile_from_string(
			XNG_GL_SHADER_FRAGMENT,
			m_preprocessedShader.get_section("fragment")->content.c_str(),
			&macrosVector[0]))
		{
			shaders.pop_back();
		}
	}

	for (shader & s : shaders)
	{
		program->attach_shader(&s);
	}

	program->link();

	for (shader & s : shaders)
	{
		program->detach_shader(&s);
	}

	if (!program->is_linked())
	{
		m_shaders.erase(it.first);
		return nullptr;
	}

	return program.get();
}

void uber_shader::free(const char * name)
{
	auto it = m_shaders.find(name);

	if (it != m_shaders.end())
	{
		m_shaders.erase(it);
	}
}