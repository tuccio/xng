#pragma once

#include <xng/core.hpp>
#include <xng/os.hpp>

namespace xng
{
    namespace graphics
    {
        class acceleration_library :
            public core::singleton<acceleration_library>
        {

        public:

            acceleration_library(void);

            os::shared_library::native_symbol_handle get_symbol_address(const char * name);

        private:

            os::shared_library m_library;

        };
    }
}