#include <xng/graphics/font.hpp>
#include <xng/core.hpp>

#include <xng/graphics/freetype_font_loader.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <fstream>

using namespace xng::graphics;
using namespace xng::res;
using namespace xng::os;

const char * font::resource_type = "font";

font::font(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	resource(name, params, loader, owner)
{
	m_pointSize = 0;
}

font::~font(void)
{
	clear();
}

bool font::load_impl(const void * userdata)
{
	return read_file(get_name());
}

void font::unload_impl(void)
{
	clear();
}

size_t font::calculate_size_impl(void)
{
	return m_image ? m_image->get_size() : 0;
}

void font::clear(void)
{
	m_glyphs.clear();
	m_image.reset();
}

image_ptr font::get_image(void) const
{
	return m_image;
}

void font::set_image(image_ptr image)
{
	m_image = image;
}

const glyph_map & font::get_glyph_map(void) const
{
	return m_glyphs;
}

void font::set_glyph_map(const glyph_map & glyphs)
{
	m_glyphs = glyphs;
}

uint32_t font::get_point_size(void) const
{
	return m_pointSize;
}

void font::set_point_size(uint32_t pointSize)
{
	m_pointSize = pointSize;
}

uint32_t font::get_spread_factor(void) const
{
	return m_spreadFactor;
}

void font::set_spread_factor(uint32_t spreadFactor)
{
	m_spreadFactor = spreadFactor;
}


bool font::write_file(const path & filename) const
{
	path xmlFile(filename);

	if (!xmlFile.has_extension())
	{
		xmlFile.replace_extension(".xml");
	}

	path pngFile = xmlFile;
	pngFile.replace_extension(".png");

	if (m_image)
	{
		std::wofstream out(xmlFile.c_str());

		if (out)
		{
			boost::property_tree::wptree fontXML;
			boost::property_tree::wptree glyphs;

			for (auto & p : m_glyphs)
			{
				boost::property_tree::wptree glyphXML;

				glyphXML.put(L"code", p.first);
				glyphXML.put(L"advance", p.second.advance);
				glyphXML.put(L"offset.x", p.second.offset.x);
				glyphXML.put(L"offset.y", p.second.offset.y);
				glyphXML.put(L"size.x", p.second.size.x);
				glyphXML.put(L"size.y", p.second.size.y);
				glyphXML.put(L"uv0.x", p.second.uv0.x);
				glyphXML.put(L"uv0.y", p.second.uv0.y);
				glyphXML.put(L"uv1.x", p.second.uv1.x);
				glyphXML.put(L"uv1.y", p.second.uv1.y);

				glyphs.add_child(L"glyph", glyphXML);
			}

			fontXML.put(L"font.spread_factor", m_spreadFactor);
			fontXML.put(L"font.point_size", m_pointSize);
			fontXML.put_child(L"font.glyphs", glyphs);
			fontXML.put(L"font.texture.source", pngFile.filename().c_str());
			fontXML.put(L"font.texture.mipmaps", 1u);

			try
			{
				boost::property_tree::write_xml(out, fontXML);
			}
			catch (boost::property_tree::xml_parser_error)
			{
				return false;
			}

			return m_image->write_png_file(pngFile);
		}
	}
	
	return false;
}

bool font::read_file(const path & filename)
{
	path xmlFile(filename);

	if (!xmlFile.has_extension())
	{
		xmlFile.replace_extension(".xml");
	}

	std::wifstream in(xmlFile.c_str());

	if (in)
	{
		boost::property_tree::wptree fontXML;
		boost::property_tree::wptree glyphs;

		try
		{
			boost::property_tree::read_xml(in, fontXML);
		}
		catch (boost::property_tree::xml_parser_error)
		{
			return false;
		}

		m_glyphs.clear();

		if (auto optGlyphs = fontXML.get_child_optional(L"font.glyphs"))
		{
			glyphs = *optGlyphs;

			for (auto & p : glyphs)
			{
				glyph g = {};

				g.code     = p.second.get(L"code", g.code);
				g.advance  = p.second.get(L"advance", g.advance);
				g.offset.x = p.second.get(L"offset.x", g.offset.x);
				g.offset.y = p.second.get(L"offset.y", g.offset.y);
				g.size.x   = p.second.get(L"size.x", g.size.x);
				g.size.y   = p.second.get(L"size.y", g.size.y);
				g.uv0.x    = p.second.get(L"uv0.x", g.uv0.x);
				g.uv0.y    = p.second.get(L"uv0.y", g.uv0.y);
				g.uv1.x    = p.second.get(L"uv1.x", g.uv1.x);
				g.uv1.y    = p.second.get(L"uv1.y", g.uv1.y);

				*m_glyphs[g.code] = g;
			}
		}

		m_spreadFactor = 0;
		m_pointSize    = 0;

		std::wstring imageFile;

		m_spreadFactor = fontXML.get(L"font.spread_factor", m_spreadFactor);
		m_pointSize    = fontXML.get(L"font.point_size", m_pointSize);
		imageFile      = fontXML.get(L"font.texture.source", imageFile);

		m_image = resource_factory::get_singleton()->create<image>(
			(xmlFile.parent_path() / imageFile).make_unix_like().string().c_str(),
			make_resource_parameters("generate_mipmaps", true));

		return m_image->load();
	}

	return false;
}

const wchar_t * xng::graphics::make_default_charset(void)
{
	// ASCII printable subset
	return L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
}