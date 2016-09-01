#include "editor.hpp"
#include "render_panel.hpp"

#include <wx/listctrl.h>

#include <xng/engine.hpp>

using namespace xng::editor;
using namespace xng::os;
using namespace xng::engine;

struct wx_app :
	public wxApp
{
	bool OnInit(void) override { return true; }
};

void create_menu(wxFrame * editor);

editor::editor(native_window * window)
{
	wxApp::SetInstance(new wx_app);

	static int argc = 1;
	static char * argv[] = { "xngeditor", nullptr };

	if (wxEntryStart(argc, argv))
	{
		math::uint2 size = window->get_window_size();

		m_editor = new wxFrame(nullptr, wxID_ANY, "XNG Editor");
		render_panel * renderPanel = new render_panel(window, m_editor, wxID_ANY);

		m_editor->Show();
		renderPanel->Show();
		window->show();

		m_auiManager = std::make_unique<wxAuiManager>();

		m_auiManager->SetManagedWindow(m_editor);
		m_auiManager->AddPane(renderPanel, wxCENTER);

		m_auiManager->GetPane(renderPanel).name = "render_panel";

		m_auiManager->Update();

		create_menu(m_editor);

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
	delete wxTheApp;
	wxApp::SetInstance(nullptr);
	
	wxEntryCleanup();
	wxUninitialize();
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
		wxSizer * vSizer = new wxBoxSizer(wxVERTICAL);
		wxSizer * hSizer = new wxBoxSizer(wxHORIZONTAL);

		wxStaticText  * text = new wxStaticText(this, wxID_ANY,
			"Pick the rendering module to switch to from this list:",
			wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);

		wxListCtrl * list   = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
		wxButton   * ok     = new wxButton(this, wxID_OK, "Ok");
		wxButton   * cancel = new wxButton(this, wxID_CANCEL, "Cancel");

		hSizer->Add(ok, 0, wxLEFT | wxRIGHT, 1);
		hSizer->Add(cancel, 0, wxLEFT | wxRIGHT, 1);

		vSizer->Add(text, 0, wxLEFT | wxRIGHT | wxTOP, 7);
		vSizer->Add(list, 1, wxEXPAND | wxALL, 5);
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

	wxMenuBar * menuBar = new wxMenuBar;

	wxMenu * fileMenu   = new wxMenu();
	wxMenu * renderMenu = new wxMenu();

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

			auto moduleDlg = new render_module_dialog();

			if (moduleDlg->factory)
			{
				game * instance = game::get_singleton();

				render_module * newModule = dynamic_cast<render_module*>(moduleDlg->factory->create());
				render_module * oldModule = instance->get_render_module();

				instance->stop_rendering();

				if (oldModule)
				{
					oldModule->shutdown();
					delete oldModule;
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
