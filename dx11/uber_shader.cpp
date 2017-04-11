#include <xng/dx11/uber_shader.hpp>
#include <xng/dx11/compile_shader.hpp>

#ifdef XNG_DX11_DEBUG
#define XNG_SHADER_OUTPUT_DEBUG(Source) { std::ofstream f("shader_fail.xhlsl"); f << (Source); }
#else
#define XNG_SHADER_OUTPUT_DEBUG(Source)
#endif

using namespace xng::dx11;
using namespace xng::graphics;

uber_shader::uber_shader(void) :
    m_preprocessed(false),
    m_flags(0),
    m_ilvFunctor([](D3D11_INPUT_ELEMENT_DESC*) {}) {}

bool uber_shader::preprocess(const char * filename)
{
    m_shaders.clear();

    m_filename = filename;
    std::vector<std::string> errors;

    shader_preprocessor pp;

    struct dx11_line_fixer :
        shader_preprocessor_observer
    {
        void on_include_begin(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) override
        {
            //os << "#line " << 0 << " \"" << includeFile << "\"" << std::endl;
            //os << "#line " << line << std::endl;
        }

        void on_include_end(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) override
        {
            //os << "#line " << line << " \"" << compiledFile << "\"" << std::endl;
            //os << "#line " << line << std::endl;
        }
    } lineFixer;

    pp.add_observer(&lineFixer);

    m_preprocessed = pp.preprocess(filename, &m_preprocessedShader, &errors);

    if (!m_preprocessed)
    {
        auto stream = XNG_LOG_STREAM();
        stream << "A list of errors was raised when preprocessing uber shader \"" << filename << "\":" << std::endl;
        for (auto & e : errors) stream << e << std::endl;
        XNG_LOG("xng::dx11::uber_shader", stream);
    }

    return m_preprocessed;
}

bool uber_shader::is_preprocessed(void) const
{
    return m_preprocessed;
}

void uber_shader::clear(void)
{
    m_flags = 0;
    m_ilvFunctor = decltype(m_ilvFunctor)();
    m_preprocessed = false;
    m_shaders.clear();
    m_filename.clear();
    m_preprocessedShader = decltype(m_preprocessedShader){};
}

shader_program uber_shader::compile(ID3D11Device * device, const char * name, const std::vector<shader_macro> & macros)
{
    auto shaderIt = m_shaders.find(name);

    if (shaderIt != m_shaders.end())
    {
        return shaderIt->second;
    }

    shader_program program;

    std::vector<shader_macro> macrosVector = macros;
    macrosVector.emplace_back(shader_macro{ nullptr, nullptr });

    if (m_preprocessedShader.has_section("vertex"))
    {
        auto section = m_preprocessedShader.get_section("vertex");
        const char * source = section->content.c_str();

        com_ptr<ID3D11VertexShader> shader;
        com_ptr<ID3D11InputLayout> inputLayout;

        if (compile_shader_from_string(
            device,
            source,
            m_filename.c_str(),
            section->entrypoint.c_str(),
            XNG_DX11_SHADER_MODEL_5_0,
            m_flags,
            (const D3D_SHADER_MACRO *)&macrosVector[0],
            &shader,
            &inputLayout,
            m_ilvFunctor))
        {
            program.set_vertex_shader(shader.get());
            program.set_input_layout(inputLayout.get());
        }
        else
        {
            XNG_SHADER_OUTPUT_DEBUG(source);
            XNG_DEBUGBREAK();
            return shader_program();
        }
    }

    if (m_preprocessedShader.has_section("pixel"))
    {
        auto section = m_preprocessedShader.get_section("pixel");
        const char * source = section->content.c_str();

        com_ptr<ID3D11PixelShader> shader;

        if (compile_shader_from_string(
            device,
            source,
            m_filename.c_str(),
            section->entrypoint.c_str(),
            XNG_DX11_SHADER_MODEL_5_0,
            0,
            (const D3D_SHADER_MACRO *)&macrosVector[0],
            &shader))
        {
            program.set_pixel_shader(shader.get());
        }
        else
        {
            XNG_SHADER_OUTPUT_DEBUG(source);
            XNG_DEBUGBREAK();
            return shader_program();
        }
    }

    m_shaders.emplace(name, program);

    return program;
}

void uber_shader::free(const char * name)
{
    auto it = m_shaders.find(name);

    if (it != m_shaders.end())
    {
        m_shaders.erase(it);
    }
}

bool uber_shader::reload(void)
{
    m_shaders.clear();
    return preprocess(m_filename.c_str());
}