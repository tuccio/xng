#include <xng/gl/uber_shader.hpp>
#include <xng/core.hpp>

#include <xng/graphics/uber_shader_preprocessor.hpp>

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

	uber_shader_preprocessor pp;

	struct gl_line_fixer :
		uber_shader_preprocessor_observer
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
	m_hashOffsets.clear();
	m_macros.clear();
	m_commonMacros.clear();
	m_shaders.clear();
	m_filename.clear();
	m_preprocessedShader = decltype(m_preprocessedShader){};
}

const shader_program * uber_shader::compile(std::initializer_list<shader_macro> macros)
{
	hash_type hash;

	if (!is_preprocessed() || !compute_hash(macros, &hash))
	{
		return nullptr;
	}

	auto it = m_shaders.find(hash);

	if (it != m_shaders.end())
	{
		return &it->second;
	}

	std::vector<shader_macro> macrosVector(macros.size() + m_commonMacros.size() + 1);

	std::transform(m_commonMacros.begin(), m_commonMacros.end(), macrosVector.begin(),
		[](const std::pair<std::string, std::string> & define)
	{
		return shader_macro{ define.first.c_str(), define.second.c_str() };
	});

	std::copy(macros.begin(), macros.end(), std::next(macrosVector.begin(), m_commonMacros.size()));

	shader_program program;
	shader_program * result = nullptr;

	std::vector<shader*> shaders;

	if (m_preprocessedShader.has_section("vertex"))
	{
		shaders.emplace_back(new shader);

		if (!shaders.back()->compile_from_string(
			XNG_GL_SHADER_VERTEX,
			m_preprocessedShader.get_section("vertex").c_str(),
			&macrosVector[0]))
		{
			return nullptr;
		}
	}

	if (m_preprocessedShader.has_section("fragment"))
	{
		shaders.emplace_back(new shader);

		if (!shaders.back()->compile_from_string(
			XNG_GL_SHADER_FRAGMENT,
			m_preprocessedShader.get_section("fragment").c_str(),
			&macrosVector[0]))
		{
			return nullptr;
		}
	}

	for (shader * s : shaders)
	{
		program.attach_shader(s);
	}

	if (program.link())
	{
		auto it = m_shaders.emplace(hash, std::move(program));
		result = &it.first->second;
	}

	for (shader * s : shaders)
	{
		program.detach_shader(s);
		delete s;
	}

	return result;
}

bool uber_shader::compute_hash(std::initializer_list<shader_macro> defines, hash_type * pHash)
{
	hash_type hash = 0;

	for (auto & define : defines)
	{

		// Binary search on the macro definitions to find the macro index
		// we use to get the bit offset in the hash built in create_hash_offsets

		auto it = std::lower_bound(m_macros.begin(), m_macros.end(), define.name,
			[](const shader_macro_domain & a, const char * b)
		{
			return std::less<std::string>()(a.macro, b);
		});

		if (it == m_macros.end() || it->macro != define.name)
		{
			XNG_LOG_DEBUG(XNG_LOG_STREAM() << "Unrecognized macro definition: \"" << define.name << "\".");
			return false;
		}
		else
		{
			size_t index = std::distance(m_macros.begin(), it);
			size_t offset = index == 0 ? 0 : m_hashOffsets[index - 1];

			auto & domain = it->domain;

			// Binary search on the domain to find if the value is valid
			auto valueIt = std::lower_bound(domain.begin(), domain.end(), define.definition);

			if (*valueIt == define.definition)
			{
				// Add 1 cause value 0 is for undefined
				size_t value = std::distance(domain.begin(), valueIt) + 1;
				hash |= value << offset;
			}
			else
			{
				XNG_LOG_DEBUG(XNG_LOG_STREAM() << "Unrecognized value for macro \"" << define.name << "\": \"" << define.definition << "\".");
				return false;
			}
		}
	}

	*pHash = hash;
	return true;
}

bool uber_shader::create_hash_offsets(void)
{
	// Creates a vector of partial sums of bits allocation, that
	// allows the computation of offsets for hashing in O(1)

	m_hashOffsets.clear();
	m_hashOffsets.reserve(m_macros.size());

	size_t currentOffset = 0;

	for (auto & macro : m_macros)
	{
		// Value 0 is for undefined, so we add 1 to the domain size
		size_t domainSize = 1 + macro.domain.size();
		size_t bits = (size_t)std::ceil(std::log2(domainSize));

		currentOffset += bits;
		m_hashOffsets.push_back(currentOffset);
	}

	if (currentOffset > sizeof(hash_type) * 8)
	{
		XNG_LOG_DEBUG(XNG_LOG_STREAM() << "Macro permutations hash values out of range.");
		return false;
	}

	return true;
}