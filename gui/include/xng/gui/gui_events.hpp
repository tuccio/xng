#pragma once

#include <xng/gui/widget.hpp>
#include <xng/core/event_handler.hpp>
#include <functional>

enum xng_gui_events
{
	XNG_GUI_EVENT_SLIDER
};

namespace xng
{
	namespace core
	{
		template <>
		struct event_callback<xng_gui_events, XNG_GUI_EVENT_SLIDER>
		{
			typedef std::function<void(const gui::widget *, float)> type;
		};
	}
}