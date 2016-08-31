#include <xng/os/native_window_observer.hpp>

using namespace xng::os;
using namespace xng::math;

void native_window_observer::on_create(native_window * wnd) {}
void native_window_observer::on_destroy(native_window * wnd) {}

void native_window_observer::on_resize(native_window * wnd, const uint2 & windowSize, const uint2 & clientSize) {}