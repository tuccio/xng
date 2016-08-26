#pragma once

#include <xng/core.hpp>
#include <unordered_map>
#include <ostream>
#include <cstdint>

namespace xng
{
	namespace graphics
	{
		struct preprocessed_uber_shader
		{
			std::unordered_map<std::string, std::string> sections;

			bool has_section(const char * section)
			{
				return sections.end() != sections.find(section);
			}

			const std::string & get_section(const char * section)
			{
				return sections.find(section)->second;
			}
		};

		struct uber_shader_preprocessor_observer
		{
			virtual void on_include_begin(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) {}
			virtual void on_include_end(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) {}
		};

		class uber_shader_preprocessor :
			public core::observable<uber_shader_preprocessor_observer>
		{

		public:

			using output_type = preprocessed_uber_shader;

			bool preprocess(const char * filename, preprocessed_uber_shader * out, std::vector<std::string> * errors, uint32_t maxIncludeDepth = 32);

		private:

			bool expand_includes_recursive(std::ostream & out, std::istream & in, const char * filename, uint32_t depth, uint32_t maxDepth, std::vector<std::string> * errors);
			void process_section(std::ostream & out, std::istream & in, const char * section, std::vector<std::string> * errors);
		};
	}
}