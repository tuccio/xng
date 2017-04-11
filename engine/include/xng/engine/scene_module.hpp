#pragma once

#include <xng/engine/module.hpp>
#include <xng/graphics.hpp>

namespace xng
{
    namespace engine
    {
        class scene_module :
            public module
        {

        public:

            virtual bool init(void) = 0;
            virtual void shutdown(void) = 0;
            virtual bool is_initialized(void) const = 0;

            virtual graphics::scene_ptr create_scene(const char * name) = 0;
            virtual bool destroy_scene(const char * name) = 0;
            virtual graphics::scene_ptr find_scene(const char * name) = 0;

            virtual graphics::scene_ptr get_active_scene(void) = 0;
            virtual void set_active_scene(const graphics::scene_ptr & scene) = 0;

        };
    }
}