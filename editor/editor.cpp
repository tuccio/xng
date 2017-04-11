#include "editor.hpp"
#include "render_panel.hpp"
#include "camera_page.hpp"
#include "light_page.hpp"

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

struct editor_window_observer :
    public native_window_observer
{

    editor_window_observer(fps_camera_controller * cameraController) : m_cameraController(cameraController) {}

    void on_resize(native_window * wnd, const uint2 & windowSize, const uint2 & clientSize) override
    {
        float ratio = clientSize.x / (float)clientSize.y;

        scene_module * sm = game::get_singleton()->get_scene_module();

        if (sm)
        {
            scene * s = sm->get_active_scene().get();

            if (s)
            {
                scene_graph_camera * cam = s->get_active_camera();

                if (cam)
                {
                    cam->get_camera()->set_aspect_ratio(ratio);
                }
            }
        }

        if (m_cameraController)
        {
            m_cameraController->set_screen_size((float2)clientSize);
        }
    }

private:

    fps_camera_controller * m_cameraController;

};

editor::editor(native_window * wnd) :
    m_selectedObjectPage(nullptr)
{
    static int argc = 1;
    static char * argv[] = { "xngeditor", nullptr };

    if (wxEntryStart(argc, argv))
    {
        wxTheApp->CallOnInit();

        m_window = wnd;
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

        create_menu();
        create_gui();

        m_editor->SetSize(wxSize(size.x, size.y));

        m_editor->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent & event)
        {
            engine::game::get_singleton()->quit();
        });

        wxTheApp->SetTopWindow(m_editor);

        // Init default scene

        module_factory * sceneFactory = module_manager::get_singleton()->
            find_module_by_name("xngbasicscene");

        if (sceneFactory)
        {
            game * instance = game::get_singleton();

            native_window * wnd = instance->get_window();

            scene_module * sceneModule = dynamic_cast<scene_module*>(sceneFactory->create());

            if (sceneModule->init())
            {
                instance->set_scene_module(sceneModule);
                load_scene("./scene/sponza.fbx");
            }

            m_observer = std::unique_ptr<native_window_observer>(xng_new editor_window_observer(&m_cameraController));
            m_window->add_observer(m_observer.get());

            m_observer->on_resize(m_window, m_window->get_window_size(), m_window->get_client_size());

            instance->get_input_handler()->keyboard().add_observer(&m_cameraController);
            instance->get_input_handler()->mouse().add_observer(&m_cameraController);
        }

        wxAuiNotebook * renderingNotebook = xng_new wxAuiNotebook(m_editor, wxID_ANY);
        wxAuiNotebook * sceneNotebook     = xng_new wxAuiNotebook(m_editor, wxID_ANY);

        renderingNotebook->Freeze();
        sceneNotebook->Freeze();

        if (m_auiManager->AddPane(renderingNotebook, wxLEFT, _("")) &&
            m_auiManager->AddPane(sceneNotebook, wxRIGHT, _("")))
        {
            m_auiManager->GetPane(sceneNotebook).MinSize(wxSize(200, 250)).Name("scene_panel");
            m_auiManager->GetPane(renderingNotebook).MinSize(wxSize(200, 250)).Name("rendering_settings_panel");

            m_sceneGraphPage        = xng_new scene_graph_page(game::get_singleton()->get_scene_module()->get_active_scene()->get_scene_graph(), m_editor);
            m_renderingSettingsPage = xng_new rendering_settings_page(m_editor);

            m_sceneGraphPage->add_observer(this);

            sceneNotebook->AddPage(m_sceneGraphPage, _("Scene Graph"));
            renderingNotebook->AddPage(m_renderingSettingsPage, _("Rendering Settings"));

            m_sceneNotebook     = sceneNotebook;
            m_renderingNotebook = renderingNotebook;

            sceneNotebook->Thaw();
            renderingNotebook->Thaw();
        }

        m_auiManager->Update();
    }
    else
    {
        XNG_LOG("XNG Editor", "Failed to initialize wxWidgets.");
    }
}

editor::~editor(void)
{
    m_sceneGraphPage->remove_observer(this);
    m_window->remove_observer(m_observer.get());

    game * instance = game::get_singleton();

    instance->get_input_handler()->keyboard().remove_observer(&m_cameraController);
    instance->get_input_handler()->mouse().remove_observer(&m_cameraController);

    m_observer.reset();

    m_auiManager->UnInit();
    m_auiManager.reset();

    m_editor->Destroy();

    wxEntryCleanup();
}

void editor::update(float dt)
{
    wxTheApp->ProcessPendingEvents();
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
    m_cameraController.on_update(dt);
}

wxFrame * editor::get_main_window(void)
{
    return m_editor;
}

bool editor::load_scene(const char * name)
{
    scene_module * sceneModule  = game::get_singleton()->get_scene_module();

    sceneModule->destroy_scene("default_scene");
    scene_ptr defaultScene = sceneModule->create_scene("default_scene");
    
    bool success = false;

    int ppsteps =
        aiProcess_CalcTangentSpace |
        aiProcess_ValidateDataStructure |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_FindDegenerates |
        aiProcess_FindInvalidData |
        aiProcess_OptimizeMeshes |
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate |
        aiProcess_FlipUVs;

    if (!assimp_load(defaultScene.get(), name, ppsteps))
    {
        XNG_LOG("xngeditor", "Failed to load scene.");
    }
    else
    {
        auto cameras = defaultScene->get_camera_nodes();

        if (!cameras.empty())
        {
            defaultScene->set_active_camera(cameras[0]);
            m_cameraController.set_camera(cameras[0]);
        }

        success = true;
    }

    sceneModule->set_active_scene(defaultScene);

    if (m_observer)
    {
        m_observer->on_resize(m_window, m_window->get_window_size(), m_window->get_client_size());
    }

    return success;
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

void editor::create_menu(void)
{
    enum {
        IDM_FILE_IMPORT,
        IDM_FILE_EXIT,
        IDM_RENDER_MODULE
    };

    wxMenuBar * menuBar = xng_new wxMenuBar;

    wxMenu * fileMenu   = xng_new wxMenu();
    wxMenu * renderMenu = xng_new wxMenu();

    menuBar->Append(fileMenu, "File");
    menuBar->Append(renderMenu, "Render");

    fileMenu->Append(IDM_FILE_IMPORT, "Import Scene", nullptr);
    fileMenu->Append(IDM_FILE_EXIT, "Exit", nullptr);

    renderMenu->Append(IDM_RENDER_MODULE, "Switch Module", nullptr);

    menuBar->Bind(wxEVT_MENU, [=](wxCommandEvent & evt)
    {
        switch (evt.GetId())
        {

        case IDM_FILE_IMPORT:
        {
            wxFileDialog * fileDlg = xng_new wxFileDialog(m_editor);

            if (fileDlg->ShowModal() == wxID_OK &&
                !load_scene(fileDlg->GetPath().c_str()))
            {
                os::message_box("Error", "Failed to import scene", XNG_MESSAGE_BOX_ERROR, m_window);
            }
        }
            break;

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
                instance->pause();

                if (newModule && newModule->init(instance->get_window()))
                {
                    instance->set_render_module(newModule);

                    if (oldModule)
                    {
                        newModule->configuration() = oldModule->configuration();

                        oldModule->shutdown();
                        xng_delete oldModule;
                    }
                }
                
                instance->unpause();
                instance->start_rendering();
            }

            moduleDlg->Destroy();
            break;

        }
    });

    m_editor->SetMenuBar(menuBar);
}

void editor::create_gui(void)
{
    game        * instance = game::get_singleton();
    gui_manager * gui      = instance->get_gui_manager();

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

    m_fpsText = fpsText;
}

void editor::on_node_select(scene_graph * sg, scene_graph_node * node)
{
    if (m_selectedObjectPage)
    {
        m_sceneNotebook->Freeze();

        size_t page = m_sceneNotebook->GetPageIndex(m_selectedObjectPage);

        if (page != wxNOT_FOUND)
        {
            m_sceneNotebook->DeletePage(page);
        }

        m_sceneNotebook->Thaw();

        m_selectedObjectPage = nullptr;
    }

    switch (node->get_node_type())
    {
    case XNG_SCENE_GRAPH_CAMERA:    

        m_selectedObjectPage = xng_new camera_page(static_cast<scene_graph_camera*>(node), m_editor);

        m_sceneNotebook->Freeze();

        m_sceneNotebook->AddPage(
            m_selectedObjectPage,
            "Camera");

        m_sceneNotebook->Thaw();

        break;

    case XNG_SCENE_GRAPH_LIGHT:

        m_selectedObjectPage = xng_new light_page(static_cast<scene_graph_light*>(node), m_editor);

        m_sceneNotebook->Freeze();

        m_sceneNotebook->AddPage(
            m_selectedObjectPage,
            "Light");

        m_sceneNotebook->Thaw();

        break;
    }
}