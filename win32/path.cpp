#include <xng/os/path.hpp>
#include <xng/core/string_convert.hpp>

#include <regex>

#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

using namespace xng::os;

static std::locale s_locale;
const path::value_type path::preferred_separator = L'\\';

const path::string_type & path::native(void) const
{
	return m_path;
}

const path::value_type * path::c_str(void) const
{
	return m_path.c_str();
}

std::string path::string(const codecvt_type & cvt) const
{
	std::string to;
	core::string_convert(m_path.c_str(), m_path.c_str() + m_path.size(), to, cvt);
	return to;
}

std::wstring path::wstring(const codecvt_type & cvt) const
{
	return m_path;
}

path path::operator/ (const path & rhs) const
{
	return path(m_path + preferred_separator + rhs.m_path);
}

path & path::operator/= (const path & rhs)
{
	m_path += preferred_separator;
	m_path += rhs.m_path;
	return *this;
}

// Comparison

bool path::operator== (const path & rhs)
{
	return m_path == rhs.m_path;
}

// Modifiers

void path::clear(void)
{
	m_path.clear();
}

void path::swap(path & rhs)
{
	m_path.swap(rhs.m_path);
}

path & path::make_preferred(void)
{
	static const std::wregex s_reSlashFix(L"[/\\\\]+");
	m_path = std::regex_replace(m_path, s_reSlashFix, L"\\");
	return *this;
}

path & path::make_unix_like(void)
{
	static const std::wregex s_reSlashFix(L"[/\\\\]+");
	m_path = std::regex_replace(m_path, s_reSlashFix, L"/");
	return *this;
}

path & path::remove_filename(void)
{
	return *this = parent_path();
}

path & path::replace_extension(const path & new_extension)
{
	if (new_extension.empty())
	{
		wchar_t * p = PathFindExtensionW(m_path.c_str());
		m_path.substr(0, p - m_path.c_str());
	}
	else
	{
		wchar_t path[MAX_PATH];

		std::wstring replacement = new_extension.m_path[0] == L'.' ?
			new_extension.m_path : (L"." + new_extension.m_path);

		StrCpyW(path, m_path.c_str());
		PathRenameExtensionW(path, replacement.c_str());

		m_path = path;
	}
	
	return *this;
}

// Decomposition

path path::extension(void) const
{
	wchar_t * p = PathFindExtensionW(m_path.c_str());
	return path(p);
}

path path::filename(void) const
{
	const wchar_t * p = PathFindFileNameW(m_path.c_str());	
	return path(p);
}

path path::parent_path(void) const
{
	const wchar_t * p = PathFindFileNameW(m_path.c_str());
	return path(m_path.c_str(), p);
}

// Query

bool path::empty(void) const
{
	return m_path.empty();
}

bool path::has_extension(void) const
{
	wchar_t * p = PathFindExtensionW(m_path.c_str());
	return *p == L'.' && (*(p + 1) != L'\0');
}

std::locale path::imbue(const std::locale & loc)
{
	std::locale old = s_locale;
	s_locale = loc;
	return old;
}

const path::codecvt_type & path::codecvt(void)
{
	return std::use_facet<path::codecvt_type>(s_locale);
}