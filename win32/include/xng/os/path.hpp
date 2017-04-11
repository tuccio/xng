#pragma once

#include <locale>
#include <string>

namespace xng
{
    namespace os
    {
        class path
        {

        public:

            typedef wchar_t                                     value_type;
            typedef std::basic_string<value_type>               string_type;
            typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

            static const value_type preferred_separator;

            path(void) = default;
            path(const path &) = default;
            path(path &&) = default;

            template <typename Source>
            path(const Source & source, const codecvt_type & cvt = codecvt());

            template <class InputIterator>
            path(InputIterator begin, InputIterator end, const codecvt_type & cvt = codecvt());

            ~path(void) = default;

            const string_type & native(void) const;
            const value_type * c_str(void) const;

            std::string string(const codecvt_type & cvt = codecvt()) const;
            std::wstring wstring(const codecvt_type & cvt = codecvt()) const;
            
            static std::locale imbue(const std::locale & loc);
            static const codecvt_type & codecvt(void);

            path   operator/  (const path & rhs) const;
            path & operator/= (const path & rhs);

            path & operator=  (const path & rhs) = default;

            // Comparison

            bool operator== (const path & rhs);

            // Modifiers

            void   clear(void);
            path & make_absolute(const path & base);
            path & make_preferred(void);
            path & make_unix_like(void);
            path & remove_filename(void);
            path & replace_extension(const path & new_extension = path());
            void   swap(path & rhs);

            // Decomposition

            path root_name(void) const;
            path root_directory(void) const;
            path root_path(void) const;
            path relative_path(void) const;
            path parent_path(void) const;
            path filename(void) const;
            path stem(void) const;
            path extension(void) const;

            // Query

            bool empty(void) const;
            bool has_root_name(void) const;
            bool has_root_directory(void) const;
            bool has_root_path(void) const;
            bool has_relative_path(void) const;
            bool has_parent_path(void) const;
            bool has_filename(void) const;
            bool has_stem(void) const;
            bool has_extension(void) const;
            bool is_absolute(void) const;
            bool is_relative(void) const;

        private:

            string_type m_path;

        };
    }
}

#include "path.inl"