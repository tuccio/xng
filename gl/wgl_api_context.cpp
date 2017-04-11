#ifdef XNG_WIN32

#include <xng/gl/wgl_api_context.hpp>
#include <xng/gl/errors.hpp>

using namespace xng;
using namespace xng::gl;

wgl_api_context::wgl_api_context(void) : m_hDC(NULL), m_hGLRC(NULL) {}

wgl_api_context::wgl_api_context(wgl_api_context && rhs)
{
    m_hDC       = rhs.m_hDC;
    m_hGLRC     = rhs.m_hGLRC;
    rhs.m_hDC   = NULL;
    rhs.m_hGLRC = NULL;
}

wgl_api_context::~wgl_api_context(void)
{
    if (is_initialized())
    {
        shutdown();
    }
}

bool wgl_api_context::init(HWND hWnd, xng_api_version version, bool debug)
{
    if (hWnd)
    {
        HDC hDC = GetDC(hWnd);

        if (hDC)
        {
            PIXELFORMATDESCRIPTOR pfd =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                32,
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,
                8,
                0,
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            int ipf = ChoosePixelFormat(hDC, &pfd);
            
            if (SetPixelFormat(hDC, ipf, &pfd))
            {
                m_hDC = hDC;

                if (version == XNG_API_DEFAULT || 
                    (!WGL_ARB_create_context_profile && !WGL_ARB_create_context))
                {
                    m_hGLRC = wglCreateContext(hDC);
                }
                else
                {
                    wgl_api_context tempContext;

                    if (tempContext.init(hWnd))
                    {
                        tempContext.make_current();

                        glewExperimental = GL_TRUE;

                        if (XNG_GL_RETURN_CHECK(glewInit()) == GLEW_OK)
                        {
                            GLint majorVersion = 0;
                            GLint minorVersion = 0;
                            GLint profileBit   = 0;
                            GLint contextFlags = 0;

                            switch (version)
                            {
                            default:
                            case XNG_API_GL_ES_2_0:
                                majorVersion = 2;
                                minorVersion = 0;
                                profileBit   = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
                                break;

                            case XNG_API_GL_ES_3_0:
                                majorVersion = 3;
                                minorVersion = 0;
                                profileBit   = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
                                break;
                            case XNG_API_GL_4_5:
                                majorVersion = 4;
                                minorVersion = 5;
                                profileBit   = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
                            }

                            if (debug)
                            {
                                contextFlags |= contextFlags;
                            }

                            GLint attribs[] = {
                                WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
                                WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
                                WGL_CONTEXT_PROFILE_MASK_ARB,  profileBit,
                                WGL_CONTEXT_FLAGS_ARB,         contextFlags,
                                0
                            };

                            m_hGLRC = wglCreateContextAttribsARB(hDC, 0, attribs);

                            on_init();
                        }
                    }
                }
            }
        }
    }

    return is_initialized();
}

void wgl_api_context::shutdown(void)
{
    release_current();
    wglDeleteContext(m_hGLRC);
    m_hGLRC = NULL;
    m_hDC   = NULL;
}

bool wgl_api_context::is_initialized(void) const
{
    return m_hGLRC != NULL;
}

void wgl_api_context::make_current(void) const
{
    wglMakeCurrent(m_hDC, m_hGLRC);
    XNG_LOG_IF(glewInit() != GLEW_OK, "Call to glewInit() failed.");
    XNG_GL_CLEAR_ERROR_STACK(); // Silencing glewInit() because it seems to add errors even when succeeding
}

void wgl_api_context::release_current(void) const
{
    wglMakeCurrent(m_hDC, NULL);
}

void wgl_api_context::swap_buffers(void) const
{
    SwapBuffers(m_hDC);
}

void wgl_api_context::use(void)
{
    make_current();
}

void wgl_api_context::dispose(void)
{
    release_current();
}

void wgl_api_context::frame_complete(void)
{
    swap_buffers();
    increase_frame_index();
}

void wgl_api_context::set_vsync(bool vsync)
{
    wglSwapIntervalEXT(vsync ? 1 : 0);
}

bool wgl_api_context::get_vsync(void) const
{
    return wglGetSwapIntervalEXT() == 0 ? false : true;
}

bool wgl_api_context::is_supported(const char * extension)
{
    return XNG_GL_RETURN_CHECK(glewIsSupported(extension)) != GL_FALSE;
}

#endif /* XNG_WIN32 */