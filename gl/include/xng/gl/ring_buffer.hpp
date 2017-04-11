#pragma once

#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/gl_headers.hpp>
#include <xng/core/pool_manager.hpp>
#include <cstdint>
#include <queue>

namespace xng
{
    namespace gl
    {
        class ring_buffer
        {

        public:

            ring_buffer(void);
            ring_buffer(const ring_buffer &) = delete;
            ring_buffer(ring_buffer &&) = default;

            bool init(gl_api_context * context, GLenum target, uint32_t size);
            void shutdown(void);

            void * allocate_buffer(uint32_t size, GLuint * offset, GLuint * allocatedSize);

            template <typename T>
            XNG_INLINE T * allocate_buffer(GLuint * offset, GLuint * size)
            {
                return (T*)allocate_buffer(sizeof(T), offset, size);
            }

            XNG_INLINE GLuint get_buffer(void) const
            {
                return m_buffer;
            }

            XNG_INLINE GLenum get_target(void) const
            {
                return m_target;
            }

        private:

            struct allocated_chunk
            {
                uint32_t offset;
                uint32_t count;
                uint64_t frameIndex;
            };

            gl_api_context * m_context;

            GLuint     m_buffer;
            uint8_t  * m_bufferMap;
            uint32_t   m_size;
            GLint      m_alignment;
            GLenum     m_target;

            core::pool_manager m_pool;
            std::queue<allocated_chunk> m_allocatedChunks;

            const allocated_chunk * allocate(uint32_t count);

        };
    }
}