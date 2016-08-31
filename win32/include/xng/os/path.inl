#include <xng/core/string_convert.hpp>

namespace xng
{
	namespace os
	{
		template <typename Source>
		path::path(Source const & source, const codecvt_type & cvt)
		{
			core::string_convert(source, m_path, cvt);
		}

		template <class InputIterator>
		path::path(InputIterator begin, InputIterator end, const codecvt_type & cvt)
		{
			core::string_convert(begin, end, m_path, cvt);
		}
	}
}