#include "tests_window.hpp"

#include <xng/engine.hpp>

using namespace xng;

struct wx_app :
	public wxApp
{
	bool OnInit(void) override {
		wxApp::OnInit(); return true;
	}
};

IMPLEMENT_APP_NO_MAIN(wx_app);
IMPLEMENT_WX_THEME_SUPPORT;

vk::test::tests_window::tests_window(os::native_window * wnd)
{
	static int argc = 1;
	static char * argv[] = { "xngvktests", nullptr };

	if (wxEntryStart(argc, argv))
	{
		wxTheApp->CallOnInit();

		m_window = wnd;
		wnd->show();

		m_main = xng_new wxFrame(nullptr, wxID_ANY, "XNG Vulkan Tests");

		m_main->Show();
		wnd->show();

		create_menu();

		m_main->Fit();

		m_main->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent & event)
		{
			engine::game::get_singleton()->quit();
		});

		wxTheApp->SetTopWindow(m_main);
	}
	else
	{
		XNG_LOG("XNG Vulkan Tests", "Failed to initialize wxWidgets.");
	}
}

vk::test::tests_window::~tests_window(void)
{
	m_main->Destroy();

	wxEntryCleanup();
}

void vk::test::tests_window::update(float dt)
{
}

void vk::test::tests_window::create_menu(void)
{
}
