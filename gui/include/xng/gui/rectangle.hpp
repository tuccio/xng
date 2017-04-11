#pragma once

#include <xng/math.hpp>

namespace xng
{
    namespace gui
    {
        struct rectangle
        {
            union
            {
                struct
                {
                    int32_t left;
                    int32_t top;
                };

                math::int2 topLeft;
            };

            union
            {
                struct
                {
                    int32_t right;
                    int32_t bottom;
                };

                math::int2 bottomRight;
            };
        };

        bool rectangle_contains(const rectangle & r, const math::int2 & x);

        int32_t rectangle_width(const rectangle & r);
        int32_t rectangle_height(const rectangle & r);

        math::int2 rectangle_size(const rectangle & r);
    }
}