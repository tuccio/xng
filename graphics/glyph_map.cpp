#include <xng/graphics/glyph_map.hpp>

using namespace xng::graphics;

glyph * glyph_map::operator[](wchar_t code)
{
    return &m_glyphs[code];
}

const glyph * glyph_map::operator[](wchar_t code) const
{
    auto it = m_glyphs.find(code);
    return it != m_glyphs.end() ? &it->second : nullptr;
}

glyph_map::iterator glyph_map::begin(void) const
{
    return m_glyphs.begin();
}

glyph_map::iterator glyph_map::end(void) const
{
    return m_glyphs.end();
}

void glyph_map::clear(void)
{
    m_glyphs.clear();
}