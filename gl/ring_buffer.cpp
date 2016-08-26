#include <xng/gl/ring_buffer.hpp>
#include <xng/gl/errors.hpp>

using namespace xng::graphics;
using namespace xng::gl;
using namespace xng::core;

ring_buffer::ring_buffer(void) :
	m_context(nullptr), m_buffer(0), m_size(0), m_bufferMap(nullptr) {}

bool ring_buffer::init(gl_api_context * context, GLenum target, uint32_t size)
{
	m_context = context;
	m_target  = target;

	GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

	XNG_GL_CHECK(glGenBuffers(1, &m_buffer));
	XNG_GL_CHECK(glBindBuffer(target, m_buffer));

	XNG_GL_CHECK(glBufferStorage(target, size, nullptr, createFlags));
	m_bufferMap = (uint8_t*) XNG_GL_RETURN_CHECK(glMapBufferRange(target, 0, size, mapFlags));

	XNG_GL_CHECK(glBindBuffer(target, 0));

	switch (target)
	{
	case GL_SHADER_STORAGE_BUFFER:
		XNG_GL_CHECK(glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &m_alignment));
		break;

	case GL_UNIFORM_BUFFER:
		XNG_GL_CHECK(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &m_alignment));
		break;

	default:
		XNG_LOG("xng::gl::ring_buffer", "Unrecognized target for buffer, assuming no alignment required.");

	case GL_ARRAY_BUFFER:
		m_alignment = 1;
	}

	m_size = size / m_alignment;
	m_pool = pool_manager(m_size);

	return m_bufferMap != nullptr;
}

void ring_buffer::shutdown(void)
{
	m_pool.clear();

	XNG_GL_CHECK(glDeleteBuffers(1, &m_buffer));

	m_context   = nullptr;
	m_buffer    = 0;
	m_bufferMap = nullptr;
}

void * ring_buffer::allocate_buffer(uint32_t size, GLuint * offset, GLuint * allocatedSize)
{
	// Allocate n = ceil(size/alignment) buffers
	uint32_t n = (size + m_alignment - 1) / m_alignment;
	const allocated_chunk * chunk = allocate(n);

	if (chunk)
	{
		GLuint bOffset = chunk->offset * m_alignment;
		*offset        = bOffset;
		*allocatedSize = chunk->count * m_alignment;
		return static_cast<void*>(m_bufferMap + bOffset);
	}

	return nullptr;
}

const ring_buffer::allocated_chunk * ring_buffer::allocate(uint32_t count)
{
	if (count > m_size)
	{
		XNG_LOG("xng::gl::ssbo_ring_buffer", "Requested allocation exceeds buffer size");
		return nullptr;
	}

	while (!m_pool.has_space(count) && !m_allocatedChunks.empty())
	{
		uint64_t frameIndex = m_allocatedChunks.front().frameIndex;

		m_context->wait_for_frame(frameIndex);

		while (!m_allocatedChunks.empty() && m_allocatedChunks.front().frameIndex == frameIndex)
		{
			m_pool.free(m_allocatedChunks.front().offset, m_allocatedChunks.front().count);
			m_allocatedChunks.pop();
		}
	}
	
	pool_size_t offset = m_pool.allocate(count);

	if (offset != XNG_POOL_INVALID)
	{
		m_allocatedChunks.push(allocated_chunk{ offset, count, m_context->get_frame_index() });
		return &m_allocatedChunks.back();
	}
	else
	{
		return nullptr;
	}
}