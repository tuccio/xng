#pragma once

#include <xng/gl/gl_headers.hpp>
#include <vector>
#include <cstdint>

namespace xng
{
	namespace gl
	{
		class counting_fence
		{

		public:

			counting_fence(void);

			uint64_t signal(void);
			bool wait(uint64_t value);

		private:

			uint64_t m_value;
			uint64_t m_lastCompleted;
			std::vector<GLsync> m_fences;

		};
	}
}