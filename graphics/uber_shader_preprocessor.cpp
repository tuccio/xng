#include <xng/graphics/uber_shader_preprocessor.hpp>

#include <fstream>
#include <regex>
#include <iterator>
#include <sstream>

using namespace xng::graphics;

bool is_section(const std::string & line, std::string * section)
{
	static std::regex re("^#xng\\s+section\\s*\\(\\s*([a-zA-Z]+)\\s*\\)\\s*$");

	std::smatch m;

	if (std::regex_match(line, m, re))
	{
		*section = m[1];
		return true;
	}

	return false;
}

bool is_include(const std::string & line, std::string * include)
{
	static std::regex re("^#xng\\s+include\\s*\\((\\s*\"([^\"]+)\"\\s*)\\)\\s*$");

	std::smatch m;

	if (std::regex_match(line, m, re))
	{
		*include = m[2];
		return true;
	}

	return false;
}

std::vector<std::string> scan_sections(std::istream & in)
{
	std::vector<std::string> v;
	std::string section;
	std::string line;
	
	while (!in.eof())
	{
		std::getline(in, line);

		if (is_section(line, &section))
		{
			v.push_back(section);
		}
	}

	return v;
}

std::string extract_path(const char * filename)
{
	std::string f(filename);
	auto it = f.find_last_of("/\\");

	if (it == std::string::npos)
	{
		return "";
	}
	
	return f.substr(0, it + 1);
}

bool uber_shader_preprocessor::preprocess(const char * filename, output_type * out, std::vector<std::string> * errors, uint32_t maxIncludeDepth)
{
	*out = output_type{};
	
	if (errors)
	{
		errors->clear();
	}

	std::ifstream in(filename);

	if (!in)
	{
		return false;
	}

	std::stringstream expandedFile;

	if (!expand_includes_recursive(expandedFile, in, filename, 0, maxIncludeDepth, errors))
	{
		return false;
	}

	expandedFile.seekg(0, std::ios::beg);
	std::vector<std::string> sections = scan_sections(expandedFile);

	for (auto & section : sections)
	{
		std::stringstream ss;

		expandedFile.clear();
		expandedFile.seekg(0, std::ios::beg);

		process_section(ss, expandedFile, section.c_str(), errors);
		out->sections.emplace(section, ss.str());
	}

	/*for (auto & section : out->sections)
	{
		XNG_LOG(section.first.c_str(), section.second.c_str());
	}*/

	return true;
}


bool uber_shader_preprocessor::expand_includes_recursive(std::ostream & out, std::istream & in, const char * filename, uint32_t depth, uint32_t maxDepth, std::vector<std::string> * errors)
{
	if (depth > maxDepth)
	{
		if (errors)
		{
			errors->emplace_back("Include files expansion exceeded max depth (might be caused by a loop in the include files)");
		}
		return false;
	}

	std::string include;
	std::string line;

	uint32_t lineNo = 0;

	std::string path = extract_path(filename);

	while (!in.eof())
	{
		std::getline(in, line);
		++lineNo;

		if (is_include(line, &include))
		{
			std::ifstream includeIS(path + include);

			if (includeIS)
			{
				std::stringstream includeOS;
				
				if (expand_includes_recursive(includeOS, includeIS, include.c_str(), depth + 1, maxDepth, errors))
				{
					notify(&uber_shader_preprocessor_observer::on_include_begin, out, filename, lineNo, include.c_str());
					std::copy(std::istreambuf_iterator<char>(includeOS), std::istreambuf_iterator<char>(), std::ostream_iterator<char>(out));
					notify(&uber_shader_preprocessor_observer::on_include_end, out, filename, lineNo, include.c_str());
				}
			}
			else if (errors)
			{
				std::string error = "Unable to open include file \"" + include;
				error += "\".";
				errors->emplace_back(std::move(error));
				return false;
			}

		}
		else
		{
			out << line << std::endl;
		}
	}

	return true;
}

void uber_shader_preprocessor::process_section(std::ostream & out, std::istream & in, const char * section, std::vector<std::string> * errors)
{
	bool write = true;
	std::string currentSection;
	std::string line;

	while (!in.eof())
	{
		std::getline(in, line);

		if (is_section(line, &currentSection))
		{
			write = (currentSection == section || currentSection == "common");
			out << std::endl;
		}
		else if (write)
		{
			out << line << std::endl;
		}
	}
}