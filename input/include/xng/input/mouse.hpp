#pragma once

#include <xng/core.hpp>
#include <xng/math.hpp>
#include <xng/input/vkeys.hpp>
#include <xng/os/timestamp.hpp>

#include <unordered_map>

namespace xng
{
    namespace input
    {
        class mouse;

        struct mouse_observer
        {
            virtual bool on_mouse_key_down(const mouse * mouse, xng_mouse_key key) { return true; }
            virtual bool on_mouse_key_up(const mouse * mouse, xng_mouse_key key, uint32_t millis) { return true; }
            virtual bool on_mouse_key_hold(const mouse * mouse, xng_mouse_key key, uint32_t millis) { return true; }
            virtual bool on_mouse_wheel(const mouse * mouse, int32_t delta) { return true; }
            virtual bool on_mouse_move(const mouse * mouse, const math::uint2 & position) { return true; }
        };

        class mouse :
            public core::observable<mouse_observer, std::mutex>
        {

        public:

            mouse(void);

            bool is_pressed(xng_mouse_key) const;

            const math::uint2 & get_position(void) const;
            int32_t get_wheel_delta(void) const;

            void clear(void);

        private:

            typedef std::unordered_map<xng_mouse_key, os::high_resolution_timestamp> mouse_map;

            mouse_map   m_keys;
            math::uint2 m_position;
            int32_t     m_wheelDelta;

            void press(xng_mouse_key key, os::high_resolution_timestamp t);
            void release(xng_mouse_key key, os::high_resolution_timestamp t);
            void release_all(os::high_resolution_timestamp t);

            void notify_held_keys(os::high_resolution_timestamp t);

            void move(const math::uint2 & position);
            void wheel(int32_t delta);
            void reset_wheel(void);

            friend class input_handler;

        };
    }
}