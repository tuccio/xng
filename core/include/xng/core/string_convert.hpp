#pragma once

#include <cwchar>
#include <locale>
#include <string>

namespace xng
{
    namespace core
    {
        typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

        // codecvt

        void string_convert(const char * begin, const char * end, std::wstring & to, const codecvt_type & cvt);
        void string_convert(const wchar_t * begin, const wchar_t * end, std::string & to, const codecvt_type & cvt);

        XNG_INLINE void string_convert(const char * begin, const char * end, std::string & to, const codecvt_type & cvt)
        {
            if (end)
            {
                to.append(begin, end);
            }
            else
            {
                to.append(begin);
            }
        }

        XNG_INLINE void string_convert(const wchar_t * begin, const wchar_t * end, std::wstring & to, const codecvt_type & cvt)
        {
            if (end)
            {
                to.append(begin, end);
            }
            else
            {
                to.append(begin);
            }
        }

        XNG_INLINE void string_convert(const char * begin, std::string & to, const codecvt_type & cvt)
        {
            return string_convert(begin, nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const wchar_t * begin, std::string & to, const codecvt_type & cvt)
        {
            return string_convert(begin, nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const char * begin, std::wstring & to, const codecvt_type & cvt)
        {
            return string_convert(begin, nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const wchar_t * begin, std::wstring & to, const codecvt_type & cvt)
        {
            return string_convert(begin, nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const std::string & from, std::wstring & to, const codecvt_type & cvt)
        {
            return string_convert(from.c_str(), nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const std::wstring & from, std::wstring & to, const codecvt_type & cvt)
        {
            return string_convert(from.c_str(), nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const std::string & from, std::string & to, const codecvt_type & cvt)
        {
            return string_convert(from.c_str(), nullptr, to, cvt);
        }

        XNG_INLINE void string_convert(const std::wstring & from, std::string & to, const codecvt_type & cvt)
        {
            return string_convert(from.c_str(), nullptr, to, cvt);
        }

        // No codecvt

        XNG_INLINE void string_convert(const char * begin, const char * end, std::wstring & to)
        {
            string_convert(begin, end, to, std::use_facet<codecvt_type>(std::locale()));
        }

        XNG_INLINE void string_convert(const wchar_t * begin, const wchar_t * end, std::string & to)
        {
            string_convert(begin, end, to, std::use_facet<codecvt_type>(std::locale()));
        }

        XNG_INLINE void string_convert(const char * begin, const char * end, std::string & to)
        {
            if (end)
            {
                to.append(begin, end);
            }
            else
            {
                to.append(begin);
            }
        }

        XNG_INLINE void string_convert(const wchar_t * begin, const wchar_t * end, std::wstring & to)
        {
            if (end)
            {
                to.append(begin, end);
            }
            else
            {
                to.append(begin);
            }
        }

        XNG_INLINE void string_convert(const char * begin, std::string & to)
        {
            return string_convert(begin, nullptr, to);
        }

        XNG_INLINE void string_convert(const wchar_t * begin, std::string & to)
        {
            return string_convert(begin, nullptr, to);
        }

        XNG_INLINE void string_convert(const char * begin, std::wstring & to)
        {
            return string_convert(begin, nullptr, to);
        }

        XNG_INLINE void string_convert(const wchar_t * begin, std::wstring & to)
        {
            return string_convert(begin, nullptr, to);
        }

        XNG_INLINE void string_convert(const std::string & from, std::wstring & to)
        {
            return string_convert(from.c_str(), nullptr, to);
        }

        XNG_INLINE void string_convert(const std::wstring & from, std::wstring & to)
        {
            return string_convert(from.c_str(), nullptr, to);
        }

        XNG_INLINE void string_convert(const std::string & from, std::string & to)
        {
            return string_convert(from.c_str(), nullptr, to);
        }

        XNG_INLINE void string_convert(const std::wstring & from, std::string & to)
        {
            return string_convert(from.c_str(), nullptr, to);
        }

    }
}