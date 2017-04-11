#pragma once

#ifdef XNG_FREETYPE

#include <xng/res.hpp>
#include <xng/graphics/freetype_initializer.hpp>
#include <xng/graphics/glyph_map.hpp>

namespace xng
{
    namespace graphics
    {
        class freetype_font_loader :
            public res::resource_loader
        {

        public:

            bool load(res::resource * r, const void * userdata) override;
            void unload(res::resource * r) override;

        };
    }
}

#endif