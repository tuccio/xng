#include "editor.hpp"
#include "render_panel.hpp"

#include <wx/listctrl.h>

#include <xng/engine.hpp>
#include <xng/gui.hpp>
#include <xng/graphics/freetype_font_loader.hpp>

#include <iomanip>

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

	std::ostringstream ss;

	ss << std::fixed << std::setprecision(2) <<
		"FPS: " << game::get_singleton()->get_frames_per_second() << std::endl <<
		"Frame Time: " << game::get_singleton()->get_frame_time() * 1000.f << " ms" << std::endl;

	auto profilerData = game::get_singleton()->get_render_module()->get_api_context()->get_profiler_data();

	for (auto & p : profilerData)
	{
		ss << p.name << ": " << p.time << " ms" << std::endl;
	}
	
	m_fpsText->set_text_and_fit(ss.str());
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

void editor::create_gui(void)
{
	game * instance = game::get_singleton();
	gui_manager * gui = instance->get_gui_manager();

	window * captionTextStyle = xng_new window(gui, nullptr, int2(24, 96), int2(256));

	vertical_layout * wndLayout = xng_new vertical_layout();

	slider * borderSlider = xng_new slider(gui, captionTextStyle);
	slider * widthSlider  = xng_new slider(gui, captionTextStyle);
	slider * scaleSlider  = xng_new slider(gui, captionTextStyle);

	text_control * borderText = xng_new text_control(gui, captionTextStyle);
	text_control * widthText = xng_new text_control(gui, captionTextStyle);
	text_control * scaleText = xng_new text_control(gui, captionTextStyle);

	borderText->set_text_and_fit("Border:");
	widthText->set_text_and_fit("Thinness:");
	scaleText->set_text_and_fit("Scale:");

	wndLayout->add(borderText, XNG_LAYOUT_EXPAND, 0, 4);
	wndLayout->add(borderSlider, XNG_LAYOUT_EXPAND, 0, 4);
	wndLayout->add(widthText, XNG_LAYOUT_EXPAND, 0, 4);
	wndLayout->add(widthSlider, XNG_LAYOUT_EXPAND, 0, 4);
	wndLayout->add(scaleText, XNG_LAYOUT_EXPAND, 0, 4);
	wndLayout->add(scaleSlider, XNG_LAYOUT_EXPAND, 0, 4);

	captionTextStyle->set_caption(L"FPS Text Style Test");

	captionTextStyle->set_layout(wndLayout);
	captionTextStyle->apply_layout();

	captionTextStyle->show();

	// FPS text

	window_style transparentWindowStyle = {};

	window * transparentWindow = xng_new window(gui, nullptr, int2(0), int2(0));

	text_control * fpsText = xng_new text_control(gui, transparentWindow, int2(8, 8));

	text_control_style fpsTextStyle = {};

	fpsTextStyle.text.font         = "./fonts/OpenSans-Regular.xml";
	fpsTextStyle.text.color        = float4(1);
	fpsTextStyle.text.border_color = float4(0, 0, 0, 1);
	fpsTextStyle.text.border_size  = 10;
	fpsTextStyle.text.thinness     = .45f;
	fpsTextStyle.text.scale        = .14f;

	fpsText->set_text_control_style(fpsTextStyle);
	fpsText->set_text_and_fit("");

	transparentWindow->show();

	text_control_style * fpsTextStyle2 = xng_new text_control_style(fpsTextStyle);

	// Slider binds

	borderSlider->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		fpsTextStyle2->text.border_size = 20.f * x;
		fpsText->set_text_control_style(*fpsTextStyle2);
	});

	widthSlider->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		fpsTextStyle2->text.thinness = x;
		fpsText->set_text_control_style(*fpsTextStyle2);
	});

	scaleSlider->bind<XNG_GUI_EVENT_SLIDER>([=](const widget * slider, float x)
	{
		fpsTextStyle2->text.scale = .01f + 10 * x;
		fpsText->set_text_control_style(*fpsTextStyle2);
	});

	// Set member variables

	m_fpsText = fpsText;
}