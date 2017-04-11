#pragma once

#include <cstdint>
#include <vector>

enum xng_layout_flags
{
    XNG_LAYOUT_NONE           = 0,
    XNG_LAYOUT_EXPAND         = 1,
    XNG_LAYOUT_VALIGN_TOP     = 1 << 1,
    XNG_LAYOUT_VALIGN_BOTTOM  = 1 << 2,
    XNG_LAYOUT_VALIGN_CENTER  = 1 << 3,
    XNG_LAYOUT_HALIGN_LEFT    = 1 << 4,
    XNG_LAYOUT_HALIGN_RIGHT   = 1 << 5,
    XNG_LAYOUT_HALIGN_CENTER  = 1 << 6
};

namespace xng
{
    namespace gui
    {
        class widget;

        class layout
        {

        public:

            layout(void);

        protected:            

            widget * m_owner;

            virtual void apply(void) = 0;

        private:

            friend class widget;
            void set_owner(widget * owner);

        };
    }
}