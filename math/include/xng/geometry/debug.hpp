#pragma once

#include <xng/geometry.hpp>

#include <type_traits>

namespace xng
{
	namespace math
	{
		bool write_octave_plot_script(const frustum & f, const char * filename);

		template <typename Iterator>
		typename std::enable_if<std::is_same<float3, typename std::remove_const<typename std::iterator_traits<Iterator>::value_type>::type>::value, bool>::type
			write_octave_plot_script(Iterator begin, Iterator end, const char * filename)
		{
			std::ofstream out(filename);

			if (!out)
			{
				return false;
			}

			out << "figure;" << std::endl;
			out << "hold on;" << std::endl;
			
			for (Iterator it = begin; it != end; ++it)
			{
				out << "plot3(" << it->x << ", " << it->y << ", " << it->z << ");" << std::endl;
			}
		}
	}
}