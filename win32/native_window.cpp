#include <xng/os/native_window.hpp>

#include <cassert>

#define XNG_WIN32_WINDOW_CLASS (TEXT("xng_window_class"))
#define XNG_WIN32_DEFAULT_TITLE (TEXT("XNG"))

using namespace xng;
using namespace xng::os;
using namespace xng::math;

static LRESULT CALLBACK win32_wndproc(HWND, UINT, WPARAM, LPARAM);

native_window::native_window(void) :
    m_hWnd(NULL) {}

native_window::native_window(native_window && rhs)
{
    m_hWnd = rhs.m_hWnd;
    rhs.m_hWnd = NULL;
}

native_window::~native_window(void)
{
    if (m_hWnd)
    {
        destroy();
    }
}

bool native_window::create(void)
{
    assert(!m_hWnd && "Creating a window when one is already owned by the object.");

    if (!is_class_registered() &&
        !register_class())
    {
        return false;
    }

    m_hWnd = CreateWindowEx(
        0,
        XNG_WIN32_WINDOW_CLASS,
        XNG_WIN32_DEFAULT_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        NULL,
        nullptr);

    if (m_hWnd)
    {
        RAWINPUTDEVICE devices[] = {
            { 0x01, 0x06, 0, m_hWnd }
            //{ 0x01, 0x02, RIDEV_NOLEGACY, m_hWnd }
        };

        RegisterRawInputDevices(devices, sizeof(devices) / sizeof(devices[0]), sizeof(RAWINPUTDEVICE));
        SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

        notify(&os::native_window_observer::on_create, this);
    }

    return m_hWnd != NULL;
}

void native_window::destroy(void)
{
    DestroyWindow(m_hWnd);
    m_hWnd = NULL;
}

bool native_window::exists(void) const
{
    return IsWindow(m_hWnd) == TRUE;
}

void native_window::show(void)
{
    ShowWindow(m_hWnd, SW_SHOW);
}

void native_window::hide(void)
{
    ShowWindow(m_hWnd, SW_HIDE);
}

void native_window::set_borderless(bool borderless)
{
    LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);

    if (borderless)
    {
        lStyle &= ~(WS_OVERLAPPEDWINDOW | WS_SYSMENU);
        LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
        lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);
    }
    else
    {
        lStyle |= WS_OVERLAPPEDWINDOW;
    }

    SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
    SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

bool native_window::is_borderless(void) const
{
    LONG dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    return (dwStyle & (WS_OVERLAPPEDWINDOW)) != 0;
}

void native_window::set_taskbar_icon(bool taskbar)
{
    LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

    if (!taskbar)
    {
        lExStyle |= WS_EX_TOOLWINDOW;
    }
    else
    {
        lExStyle &= ~(WS_EX_TOOLWINDOW);
    }

    SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);
    SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

bool native_window::has_taskbar_icon(void) const
{
    LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
    return (lExStyle & (WS_EX_TOOLWINDOW)) != 0;
}

typename native_window::native_handle native_window::get_native_handle(void)
{
    return m_hWnd;
}

void native_window::set_caption(const char * title)
{
    SetWindowTextA(m_hWnd, title);
}

std::string native_window::get_caption(void) const
{
    std::string s(1024, '\0');
    GetWindowTextA(m_hWnd, &s[0], 1024);
    return s;
}

math::uint2 native_window::get_position(void) const
{
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    return math::uint2(rect.left, rect.top);
}

void native_window::set_position(const math::uint2 & position)
{
    SetWindowPos(m_hWnd, 0, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

math::uint2 native_window::get_window_size(void) const
{
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    return math::uint2(rect.right - rect.left, rect.bottom - rect.top);
}

void native_window::set_window_size(const math::uint2 & size)
{
    SetWindowPos(m_hWnd, 0, 0, 0, size.x, size.y, SWP_NOMOVE | SWP_NOZORDER);
}

math::uint2 native_window::get_client_size(void) const
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    return math::uint2(rect.right - rect.left, rect.bottom - rect.top);
}

void native_window::set_client_size(const math::uint2 & size)
{
    LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    RECT rect = { 0, 0, size.x, size.y };
    AdjustWindowRect(&rect, lStyle, FALSE);
    SetWindowPos(m_hWnd, 0, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER);
}

/* Window class */

static ATOM s_class;

bool native_window::is_class_registered(void)
{
    return s_class != 0;
}

bool native_window::register_class(void)
{
    assert(!is_class_registered() && "Window class already registered.");

    WNDCLASSEX wndClass;

    wndClass.cbSize        = sizeof(WNDCLASSEX);
    wndClass.style         = 0;
    wndClass.lpfnWndProc   = native_window::wndproc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = NULL;
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = XNG_WIN32_WINDOW_CLASS;
    wndClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    s_class = RegisterClassEx(&wndClass);
    
    return is_class_registered();
}

void native_window::unregister_class(void)
{
    UnregisterClass(XNG_WIN32_WINDOW_CLASS, NULL);
}

native_window * native_window::get_window_object(HWND hWnd)
{
    return (native_window*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
}