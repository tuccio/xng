#pragma once

#include <xng/math.hpp>
#include <unordered_map>

namespace xng
{
	namespace graphics
	{
		struct glyph
		{
			wchar_t      code;
			uint32_t     advance;
			math::int2   offset;
			math::int2   size;
			math::float2 uv0;
			math::float2 uv1;
		};

		class glyph_map
		{

		public:

			typedef std::unordered_map<wchar_t, glyph>::const_iterator iterator;

			glyph_map(void) = default;
			glyph_map(const glyph_map &) = default;

			glyph * operator[] (wchar_t code);
			const glyph * operator[] (wchar_t code) const;

			iterator begin(void) const;
			iterator end(void) const;

			void clear(void);

		private:

			std::unordered_map<wchar_t, glyph> m_glyphs;

		};
	}
}