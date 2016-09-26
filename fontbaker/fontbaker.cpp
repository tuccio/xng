#include <xng/os.hpp>
#include <xng/res.hpp>
#include <xng/graphics.hpp>
#include <xng/graphics/freetype_font_loader.hpp>

#include <tclap/CmdLine.h>

using namespace xng::graphics;
using namespace xng::res;

int main(int argc, char * argv[])
{
	resource_parameters params;

	std::string inputFile;
	std::string outputFile;

	try
	{
		TCLAP::CmdLine cmd("XNG Font Baker", ' ', "1.0");

		TCLAP::ValueArg<std::string> in("i", "input", "TrueType Font to convert", true, "", "string");
		TCLAP::ValueArg<std::string> out("o", "output", "Output XML file", true, "", "string");

		TCLAP::ValueArg<uint32_t> size("s", "size", "Font size", false, 16, "int");
		TCLAP::ValueArg<uint32_t> spread("x", "spread", "Spread factor", false, 4, "int");
		TCLAP::ValueArg<uint32_t> supersample("S", "samples", "Number of samples", false, 1, "int");

		cmd.add(in);
		cmd.add(out);

		cmd.add(size);
		cmd.add(spread);
		cmd.add(supersample);

		cmd.parse(argc, argv);

		inputFile  = in.getValue();
		outputFile = out.getValue();

		size_t sizeValue        = size.getValue();
		size_t spreadValue      = spread.getValue();
		size_t supersampleValue = supersample.getValue();

		params.put("size", sizeValue);
		params.put("spread_factor", spreadValue);
		params.put("samples", supersampleValue);
	}
	catch (TCLAP::ArgException & e)
	{
		std::cout << "ERROR: " << e.error() << " " << e.argId() << std::endl;
		return 1;
	}

	xng_new xng::core::logger(xng_new xng::os::debug_ostream);

	resource_factory * rf = xng_new resource_factory;

	rf->register_manager(xng_new image_manager);
	rf->register_manager(xng_new font_manager);

	font_ptr fnt = rf->create<font>(
		inputFile.c_str(),
		params,
		resource_loader_ptr(xng_new freetype_font_loader));

	if (fnt->load())
	{
		fnt->write_file(outputFile.c_str());
	}

	return 0;
}