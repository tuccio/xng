#pragma once

#include <xng/graphics/api_context.hpp>
#include <xng/gl/gl_headers.hpp>
#include <xng/gl/counting_fence.hpp>

#define XNG_GL_CONTEXT_SYNC_FENCES 3

namespace xng
{
	namespace gl
	{
		class gl_api_context :
			public graphics::api_context
		{

		public:

			gl_api_context(void);

			virtual bool is_supported(const char * extension) = 0;

			void frame_start(void) override;

			bool wait_for_frame(uint64_t frameIndex);

			XNG_INLINE uint64_t get_frame_index(void) const
			{
				return m_frameIndex;
			}

		protected:

			void on_init(void);
			void increase_frame_index(void);

		private:

			uint64_t       m_frameIndex;
			counting_fence m_fence;

			GLint    m_majorVersion;
			GLint    m_minorVersion;

			bool     m_syncObjects;

		};
	}
}