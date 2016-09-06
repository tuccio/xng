#include <xng/os/native_window_observer.hpp>

using namespace xng::os;
using namespace xng::math;

void native_window_observer::on_create(native_window * wnd) {}
void native_window_observer::on_destroy(native_window * wnd) {}

void native_window_observer::on_resize(native_window * wnd, const uint2 & windowSize, const uint2 & clientSize) {}

void native_window_observer::on_focus(native_window * wnd) {}
void native_window_observer::on_unfocus(native_window * wnd) {}

void native_window_observer::on_keyboard_key_down(native_window * wnd, xng_keyboard_key key) {}
void native_window_observer::on_keyboard_key_up(native_window * wnd, xng_keyboard_key key) {}