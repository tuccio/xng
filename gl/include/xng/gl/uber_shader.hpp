#pragma once

#include <xng/gl/shader_program.hpp>
#include <xng/graphics/uber_shader_preprocessor.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace xng
{
	namespace gl
	{
		struct shader_macro_domain
		{
			std::string              macro;
			std::vector<std::string> domain;

			void sort(void)
			{
				std::sort(domain.begin(), domain.end());
			}
		};
		
		class uber_shader
		{

		public:

			uber_shader(void);

			void clear(void);

			bool preprocess(const char * filename);
			bool is_preprocessed(void) const;
			const shader_program * compile(std::initializer_list<shader_macro> macros = {});

		private:

			using hash_type = uint64_t;

			std::vector<hash_type>                           m_hashOffsets;
			std::vector<shader_macro_domain>                 m_macros;
			std::vector<std::pair<std::string, std::string>> m_commonMacros;

			std::unordered_map<hash_type, shader_program>    m_shaders;
			std::string                                      m_filename;

			graphics::uber_shader_preprocessor::output_type  m_preprocessedShader;
			bool                                             m_preprocessed;

			// Internal functions

			bool compute_hash(std::initializer_list<shader_macro> defines, hash_type * pHash);
			bool create_hash_offsets(void);

		};
	}
}