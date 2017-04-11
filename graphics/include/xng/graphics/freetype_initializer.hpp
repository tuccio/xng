#pragma once

#ifdef XNG_FREETYPE

#include <ft2build.h>
#include FT_FREETYPE_H

namespace xng
{
    namespace graphics
    {
        struct freetype_initializer
        {

        public:

            freetype_initializer(void)
            {
                m_error = FT_Init_FreeType(&m_library);
            }

            ~freetype_initializer(void)
            {
                if (m_error == FT_Err_Ok)
                {
                    FT_Done_FreeType(m_library);
                }
            }

            operator bool() const
            {
                return m_error == FT_Err_Ok;
            }

            operator FT_Library() const
            {
                return m_library;
            }

        private:

            FT_Library m_library;
            FT_Error   m_error;
        };
    }
}

#endif