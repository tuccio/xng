#pragma once

#include <xng/graphics/api_context.hpp>

namespace xng
{
	namespace gl
	{
		class gl_api_context :
			public graphics::api_context
		{

		public:

			virtual bool is_supported(const char * extension) = 0;

		};
	}
}