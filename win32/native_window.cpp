#include <xng/win32/native_window.hpp>

#include <cassert>

#define XNG_WIN32_WINDOW_CLASS (TEXT("xng_window_class"))
#define XNG_WIN32_DEFAULT_TITLE (TEXT("XNG"))

using namespace xng;
using namespace xng::win32;

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
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
		notify(&os::window_observer::on_create, this);
	}	

	return m_hWnd != NULL;
}

void native_window::destroy(void)
{
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
}

void native_window::show(void)
{
	ShowWindow(m_hWnd, SW_SHOW);
}

void native_window::hide(void)
{
	ShowWindow(m_hWnd, SW_HIDE);
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

glm::ivec2 native_window::get_window_size(void)
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	return glm::ivec2(rect.right - rect.left, rect.bottom - rect.top);
}

glm::ivec2 native_window::get_client_size(void)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return glm::ivec2(rect.right - rect.left, rect.bottom - rect.top);
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

/* Messages */

LRESULT CALLBACK native_window::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_DESTROY:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::window_observer::on_destroy, wnd);
		break;
	}

	case WM_SIZE:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::window_observer::on_resize, wnd, wnd->get_window_size(), wnd->get_client_size());
		break;
	}
	};

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

native_window * native_window::get_window_object(HWND hWnd)
{
	return (native_window*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
}