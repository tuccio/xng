#pragma once

#include <xng/core.hpp>
#include <unordered_map>
#include <ostream>
#include <cstdint>

namespace xng
{
    namespace graphics
    {
        struct shader_preprocessor_observer
        {
            virtual void on_include_begin(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) {}
            virtual void on_include_end(std::ostream & os, const char * compiledFile, uint32_t line, const char * includeFile) {}
        };

        class shader_preprocessor :
            public core::observable<shader_preprocessor_observer>
        {

        public:

            struct preprocessed_shader;
            using output_type = preprocessed_shader;

            bool preprocess(const char * filename, output_type * out, std::vector<std::string> * errors, uint32_t maxIncludeDepth = 32);

        private:

            bool expand_includes_recursive(std::ostream & out, std::istream & in, const char * filename, uint32_t depth, uint32_t maxDepth, std::vector<std::string> * errors);
            void process_section(std::ostream & out, std::istream & in, const char * section, std::vector<std::string> * errors);
        };

        struct shader_preprocessor::preprocessed_shader
        {
            struct section
            {
                std::string name;
                std::string content;
                std::string entrypoint;
            };

            bool has_section(const char * section) const
            {
                return sections.end() != sections.find(section);
            }

            const section * get_section(const char * section) const
            {
                return &sections.find(section)->second;
            }

            std::unordered_map<std::string, section> sections;
        };
    }
}