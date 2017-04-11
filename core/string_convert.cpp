#include <xng/core/string_convert.hpp>

namespace xng
{
    namespace core
    {
        void string_convert(const wchar_t * begin, const wchar_t * end, std::string & to, const codecvt_type & cvt)
        {
            if (!end)
            {
                for (end = begin; *end; ++end);
            }

            to = std::string(std::distance(begin, end) * cvt.max_length(), 0);
            std::mbstate_t state;
            const wchar_t * from_next;
            char * to_next;
            const codecvt_type::result result = cvt.out(state, begin, end, from_next, &to[0], &to[0] + to.size(), to_next);
            to.erase(to.begin() + std::distance(&to[0], to_next), to.end());
        }

        void string_convert(const char * begin, const char * end, std::wstring & to, const codecvt_type & cvt)
        {
            if (!end)
            {
                for (end = begin; *end; ++end);
            }

            to = std::wstring(std::distance(begin, end) * cvt.max_length(), 0);
            std::mbstate_t state;
            const char * from_next;
            wchar_t * to_next;
            const codecvt_type::result result = cvt.in(state, begin, end, from_next, &to[0], &to[0] + to.size(), to_next);
            to.erase(to.begin() + std::distance(&to[0], to_next), to.end());
        }

    }
}