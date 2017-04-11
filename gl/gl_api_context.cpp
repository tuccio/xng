#include <xng/gl/gl_api_context.hpp>
#include <xng/gl/errors.hpp>
#include <xng/gl/counting_fence.hpp>

using namespace xng::gl;

gl_api_context::gl_api_context(void) :
    m_frameIndex(1),
    m_syncObjects(false) {}

void gl_api_context::frame_start(void)
{
    if (m_frameIndex > 3)
    {
        wait_for_frame(m_frameIndex - 3);
    }
}

bool gl_api_context::wait_for_frame(uint64_t frameIndex)
{
    if (m_syncObjects)
    {
        return m_fence.wait(frameIndex);
    }

    return false;
}

void gl_api_context::on_init(void)
{
    use();

    XNG_GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &m_majorVersion));
    XNG_GL_CHECK(glGetIntegerv(GL_MINOR_VERSION, &m_minorVersion));
    
    m_syncObjects = is_supported("GL_ARB_sync");
    
    if (!m_syncObjects)
    {
        XNG_LOG("xng::gl::gl_api_context", "Sync objects are unsopported in current OpenGL context.");
    }
}

void gl_api_context::increase_frame_index(void)
{
    ++m_frameIndex;

    if (m_syncObjects)
    {
        m_fence.signal();
    }
}