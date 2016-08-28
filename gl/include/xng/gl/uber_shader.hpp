#pragma once

#include <xng/gl/shader_program.hpp>
#include <xng/graphics/shader_preprocessor.hpp>
#include <xng/graphics/shader_types.hpp>

#include <string>
#include <unordered_map>
#include <memory>

namespace xng
{
	namespace gl
	{		
		class uber_shader
		{

		public:

			uber_shader(void);

			void clear(void);

			bool preprocess(const char * filename);
			bool is_preprocessed(void) const;

			const shader_program * compile(const char * name = "", std::initializer_list<graphics::shader_macro> macros = {});
			void free(const char * name);

		private:

			std::string m_filename;

			std::unordered_map<const char *, std::unique_ptr<shader_program>> m_shaders;

			graphics::shader_preprocessor::output_type m_preprocessedShader;
			bool m_preprocessed;

		};
	}
}