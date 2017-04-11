#include <xng/gl/counting_fence.hpp>
#include <xng/gl/errors.hpp>

using namespace xng::gl;

counting_fence::counting_fence(void) : m_value(0), m_lastCompleted(0) {}

uint64_t counting_fence::signal(void)
{
    m_fences.push_back(XNG_GL_RETURN_CHECK(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0)));
    return ++m_value;
}

bool counting_fence::wait(uint64_t value)
{
    //assert(value > m_value && "Waiting for future signals is unsupported.");

    if (value <= m_lastCompleted)
    {
        return true;
    }

    uint32_t delta = value - m_lastCompleted;
    uint32_t index = delta - 1;
    uint32_t size = m_fences.size();

    GLenum r = XNG_GL_RETURN_CHECK(glClientWaitSync(m_fences[index], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED));

    if (r == GL_CONDITION_SATISFIED || r == GL_ALREADY_SIGNALED)
    {
        for (int i = 0; i < delta; ++i)
        {
            glDeleteSync(m_fences[i]);
        }

        m_fences.erase(m_fences.begin(), m_fences.begin() + delta);
        m_lastCompleted = value;

        return true;
    }

    return false;
}