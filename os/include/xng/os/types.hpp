#pragma once

namespace xng
{
	namespace os
	{

		enum message_box_type
		{
			MESSAGE_BOX_DEFAULT,
			MESSAGE_BOX_ERROR,
			MESSAGE_BOX_ALERT
		};

		typedef void(*main_loop_idle_cb) (void);

	}
}