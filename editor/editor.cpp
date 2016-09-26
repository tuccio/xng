#include "editor.hpp"
#include "render_panel.hpp"

#include <wx/listctrl.h>

#include <xng/engine.hpp>
#include <xng/gui.hpp>
#include <xng/graphics/freetype_font_loader.hpp>

using namespace xng::editor;
using namespace xng::os;
using namespace xng::engine;
using namespace xng::gui;
using namespace xng::math;
using namespace xng::input;
using namespace xng::graphics;
using namespace xng::res;

struct wx_app :
	public wxApp
{
	bool OnInit(void) override { wxApp::OnInit(); return true; }
};

IMPLEMENT_APP_NO_MAIN(wx_app);
IMPLEMENT_WX_THEME_SUPPORT;

void create_menu(wxFrame * editor);
void create_gui(void);

editor::editor(native_window * wnd)
{
	static int argc = 1;
	static char * argv[] = { "xngeditor", nullptr };

	if (wxEntryStart(argc, argv))
	{
		wxTheApp->CallOnInit();

		wnd->show();

		math::uint2 size = wnd->get_window_size();

		m_editor = xng_new wxFrame(nullptr, wxID_ANY, "XNG Editor");
		render_panel * renderPanel = xng_new render_panel(wnd, m_editor, wxID_ANY);

		m_editor->Show();
		renderPanel->Show();
		wnd->show();

		m_auiManager = std::make_unique<wxAuiManager>();

		m_auiManager->SetManagedWindow(m_editor);
		m_auiManager->AddPane(renderPanel, wxCENTER);

		m_auiManager->GetPane(renderPanel).name = "render_panel";

		m_auiManager->Update();

		create_menu(m_editor);
		create_gui();

		m_editor->SetSize(wxSize(size.x, size.y));

		m_editor->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent & event)
		{
			engine::game::get_singleton()->quit();
		});

		wxTheApp->SetTopWindow(m_editor);
	}
	else
	{
		XNG_LOG("XNG Editor", "Failed to initialize wxWidgets.");
	}
}

editor::~editor(void)
{
	m_auiManager->UnInit();
	m_auiManager.reset();

	m_editor->Destroy();

	wxEntryCleanup();
}

void editor::update(void)
{
	wxTheApp->ProcessIdle();
}

wxFrame * editor::get_main_window(void)
{
	return m_editor;
}

struct render_module_dialog :
	public wxDialog
{
	render_module_dialog(void) :
		wxDialog(nullptr, wxID_ANY, "Render Module"),
		factory(nullptr)
	{
		wxSizer * vSizer = xng_new wxBoxSizer(wxVERTICAL);
		wxSizer * hSizer = xng_new wxBoxSizer(wxHORIZONTAL);

		wxStaticBoxSizer * box = xng_new wxStaticBoxSizer(wxVERTICAL, this,
			"Pick the rendering module to switch to from this list:");

		wxListCtrl * list   = xng_new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
		wxButton   * ok     = xng_new wxButton(this, wxID_OK, "Ok");
		wxButton   * cancel = xng_new wxButton(this, wxID_CANCEL, "Cancel");

		hSizer->Add(ok, 0, wxLEFT | wxRIGHT, 1);
		hSizer->Add(cancel, 0, wxLEFT | wxRIGHT, 1);

		box->Add(list, 1, wxEXPAND | wxALL, 5);

		vSizer->Add(box, 0, wxLEFT | wxRIGHT | wxTOP, 7);
		vSizer->Add(hSizer, 0, wxALIGN_CENTER | wxBOTTOM, 5);

		list->SetMinSize(wxSize(400, 250));

		list->AppendColumn("Name", wxLIST_FORMAT_LEFT, 100);
		list->AppendColumn("Description", wxLIST_FORMAT_LEFT, 250);

		auto modules = module_manager::get_singleton()->get_modules();

		modules.erase(std::remove_if(modules.begin(), modules.end(), [](module_factory * factory)
		{
			return factory->type() != XNG_MODULE_TYPE_RENDER;
		}), modules.end());

		std::vector<wxListItem> items(modules.size() * 2);

		for (int id = 0; id < modules.size(); ++id)
		{
			int item = 2 * id;

			items[item].SetColumn(0);
			items[item].SetText(modules[id]->name());

			items[item].SetId(id);

			items[item + 1].SetColumn(1);
			items[item + 1].SetText(modules[id]->description());
			items[item + 1].SetId(id);

			list->InsertItem(items[item]);
			list->SetItem(items[item + 1]);
		}

		SetSizerAndFit(vSizer);

		if (ShowModal() == wxID_OK)
		{
			int itemIndex = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

			if (itemIndex != wxNOT_FOUND)
			{
				factory = modules[itemIndex];
			}
		}
	}

	module_factory * factory;
};

void create_menu(wxFrame * editor)
{
	enum {
		IDM_FILE_EXIT,
		IDM_RENDER_MODULE
	};

	wxMenuBar * menuBar = xng_new wxMenuBar;

	wxMenu * fileMenu   = xng_new wxMenu();
	wxMenu * renderMenu = xng_new wxMenu();

	menuBar->Append(fileMenu, "File");
	menuBar->Append(renderMenu, "Render");

	fileMenu->Append(IDM_FILE_EXIT, "Exit", nullptr);

	renderMenu->Append(IDM_RENDER_MODULE, "Switch Module", nullptr);

	menuBar->Bind(wxEVT_MENU, [=](wxCommandEvent & evt)
	{
		switch (evt.GetId())
		{

		case IDM_FILE_EXIT:

			game::get_singleton()->quit();
			break;

		case IDM_RENDER_MODULE:

			auto moduleDlg = xng_new render_module_dialog();

			if (moduleDlg->factory)
			{
				game * instance = game::get_singleton();

				render_module * newModule = dynamic_cast<render_module*>(moduleDlg->factory->create());
				render_module * oldModule = instance->get_render_module();

				instance->stop_rendering();

				if (oldModule)
				{
					oldModule->shutdown();
					xng_delete oldModule;
				}

				if (newModule && newModule->init(instance->get_window()))
				{
					instance->set_render_module(newModule);
				}
				
				instance->start_rendering();
			}

			moduleDlg->Destroy();
			break;

		}
	});

	editor->SetMenuBar(menuBar);
}

void create_gui(void)
{
	game * instance = game::get_singleton();
	gui_manager * gui = instance->get_gui_manager();

	window * test1 = xng_new window(gui, nullptr, int2(16), int2(256));
	window * test2 = xng_new window(gui, test1, int2(256, 64), int2(250, 150));
	window * test3 = xng_new window(gui, test1, int2(128, 128), int2(250, 450));

	test1->set_caption(L"Simple caption text 1");
	test2->set_caption(L"Simple caption text 2");
	test3->set_caption(L"Simple caption text 3");

	test3->set_relative(false);

	vertical_layout * t1layout = xng_new vertical_layout();

	image_control * testImage = xng_new image_control(gui, test1);

	t1layout->add(testImage, XNG_LAYOUT_EXPAND, 1, 0);

	vertical_layout * t3layout = xng_new vertical_layout();

	slider * testSlider1 = xng_new slider(gui, test3);
	slider * testSlider2 = xng_new slider(gui, test3);
	slider * testSlider3 = xng_new slider(gui, test3);
	slider * testSlider4 = xng_new slider(gui, test3);
	slider * testSlider5 = xng_new slider(gui, test3);

	testSlider1->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		style s = gui->get_style();
		s.slider_bar_height = 5 + (int)(20 * x);
		gui->set_style(s);
	});

	testSlider2->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		style s = gui->get_style();
		s.slider_size = (uint2)(float2(5.f + 25.f * x));
		gui->set_style(s);
	});

	testSlider3->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		style s = gui->get_style();
		s.caption_text_border_size = 10.f * x;
		gui->set_style(s);
	});

	testSlider4->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		style s = gui->get_style();
		s.caption_text_width = x;
		gui->set_style(s);
	});

	testSlider5->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		style s = gui->get_style();
		s.caption_text_scale = .01f + 10 * x;
		gui->set_style(s);
	});

	t3layout->add(testSlider1, XNG_LAYOUT_EXPAND, 0, 8);
	t3layout->add(testSlider2, XNG_LAYOUT_EXPAND, 0, 8);
	t3layout->add(testSlider3, XNG_LAYOUT_EXPAND, 0, 8);
	t3layout->add(testSlider4, XNG_LAYOUT_EXPAND, 0, 8);
	t3layout->add(testSlider5, XNG_LAYOUT_EXPAND, 0, 8);

	test1->set_layout(t1layout);
	test1->apply_layout();

	test3->set_layout(t3layout);
	test3->apply_layout();

	test1->show();

	/*font_ptr openSans16 = resource_factory::get_singleton()->create<font>(
		"./fonts/OpenSans-Regular.ttf",
		make_resource_parameters(),
		resource_loader_ptr(xng_new freetype_font_loader));

	if (openSans16->load())
	{
		openSans16->write_file("./fonts/OpenSans-Regular-16.xml");
	}*/

	/*font_ptr sourceSans = resource_factory::get_singleton()->create<font>(
		"./fonts/SourceSansPro-Regular.ttf",
		make_resource_parameters(),
		resource_loader_ptr(xng_new freetype_font_loader));	

	if (sourceSans->load())
	{
		sourceSans->write_file("./fonts/SourceSansPro-Regular-16.xml");
	}*/

	/*style s = gui->get_style();
	strcpy(s.caption_font, "./fonts/SourceSansPro-Regular-16.xml");
	gui->set_style(s);*/
}