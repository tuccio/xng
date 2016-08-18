#include <xng/os.hpp>

static xng::os::window     g_mainWindow;
static xng::os::main_loop  g_mainLoop;
static xng::os::gl_context g_glContext;

struct editor_window_observer :
	public xng::os::window_observer
{
	void on_create(xng::os::window * wnd) override
	{
		if (!g_glContext.init(&g_mainWindow, xng::gl::XNG_GL_VERSION_ES_2_0))
		{
			xng::os::message_box("Error", "Unable to create GL context.", xng::os::MESSAGE_BOX_ERROR, &g_mainWindow);
			wnd->destroy();
		}

		g_glContext.make_current();
		
		OutputDebugStringA("GL Version: ");
		OutputDebugStringA((LPCSTR) glGetString(GL_VERSION));
		OutputDebugStringA("\n");
	}

	void on_destroy(xng::os::window * wnd) override
	{
		g_mainLoop.quit();
		g_glContext.shutdown();
	}
};

void on_idle(void)
{
	auto clientSize = g_mainWindow.get_client_size();
	glViewport(0, 0, clientSize.x, clientSize.y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g_glContext.swap_buffers();
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	editor_window_observer wndObserver;

	g_mainWindow.add_observer(&wndObserver);

	if (!g_mainWindow.create())
	{
		xng::os::message_box("Error", "Unable to create the main window.", xng::os::MESSAGE_BOX_ERROR, &g_mainWindow);
		return 1;
	}
	
	g_mainWindow.show();

	g_mainLoop.set_idle_callback(&on_idle);
	g_mainLoop.run();

	return 0;
}

