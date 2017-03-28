#include <xng/geometry/debug.hpp>

#include <iostream>
#include <fstream>

bool xng::math::write_octave_plot_script(const frustum & f, const char * filename)
{
	std::ofstream out(filename);

	if (!out)
	{
		return false;
	}

	auto corners = f.get_corners();
	
	aabb bb = make_aabb(corners.begin(), corners.end());

	float3 m = bb.get_min();
	float3 M = bb.get_max();

	// http://stackoverflow.com/questions/23041366/how-to-plot-2-planes-in-gnu-octave

	out << "X = linspace(" << m.x << ", " << M.x << ");" << std::endl;
	out << "Y = linspace(" << m.y << ", " << M.y << ");" << std::endl;

	out << "[ XX, YY ] = meshgrid(X, Y);" << std::endl;

	for (int i = 0; i < 6; ++i)
	{
		out << "ZZ" << i << " = " << f.planes[i].d << " + (" << - f.planes[i].a / f.planes[i].c << " * XX) + (" << - f.planes[i].b / f.planes[i].c << " * YY);" << std::endl;
	}

	out << "figure;" << std::endl;
	out << "hold on;" << std::endl;

	for (int i = 0; i < 6; ++i)
	{
		out << "mesh(XX, YY, ZZ" << i << ");" << std::endl;
	}

	return true;
}