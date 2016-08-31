#pragma once

#include <xng/engine/module.hpp>

namespace xng
{
	namespace engine
	{
		class runtime_module :
			public module
		{

		public:

			virtual bool init(void) = 0;
			virtual void shutdown(void) = 0;
			virtual bool is_initialized(void) const = 0;

			virtual void update(float dt) = 0;

		};
	}
}