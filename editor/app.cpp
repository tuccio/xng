#include "wx.hpp"
#include "frame.hpp"

#include "globals.hpp"

#include <xng/core.hpp>
#include <xng/gl.hpp>
#include <xng/res.hpp>
#include <xng/dx11.hpp>
#include <xng/win32.hpp>

#include <fstream>

#include <mutex>

namespace xng
{
	namespace editor
	{
		class app :
			public wxApp
		{

		public:

			app(void) :
				m_editor(nullptr),
				m_debug(false),
				m_timer(this)
			{
#ifdef XNG_DEBUG
				m_debug = true;
#endif
			}

			~app(void) {}

			bool OnInit(void) override
			{
#ifdef XNG_WIN32
				m_logStream = std::unique_ptr<std::basic_ostream<char>>(new win32::debug_ostream);
				m_logger    = std::make_unique<core::logger>(m_logStream.get());
#else
				m_logStream = std::unique_ptr<std::basic_ostream<char>>(new std::ofstream("xng.log"));
				m_logger    = std::make_unique<core::logger>(m_logStream.get());
#endif
				res::resource_ptr<res::resource> resource;

				m_editor = new frame("XNG Editor", wxDefaultPosition, wxSize(1280, 720));
				m_panel  = m_editor->get_render_panel();

				initialize_resource_managers();

				g_scene = std::make_unique<graphics::scene>();

				// Initialize API

				m_renderMutex.lock();
				
				if (!initialize_api(m_panel->GetHandle(), XNG_API_GL_4_5, m_debug) ||
					!initialize_renderer(XNG_API_GL_4_5))
				{
					wxMessageBox("Unable to initialize graphics API.", "Error", wxICON_ERROR);
					return false;
				}
				else
				{
					set_render_loop_status(true);

					m_editor->Maximize();
					m_editor->Show(true);

					wxSize size = m_panel->GetSize();
					g_renderer->get_configuration()->set_render_resolution(xng::math::uint2(size.x, size.y));
					g_apiContext->on_resize(size.x, size.y);

					create_menu();
					update_title();

					return true;
				}
			}

			int OnExit(void) override
			{
				g_renderer.reset();
				g_apiContext.reset();
				m_logger.reset();
				m_logStream.reset();
				return 0;
			}

			bool initialize_renderer(xng_api_version api)
			{
				switch (api)
				{

				case XNG_API_GL_ES_2_0:
				case XNG_API_GL_ES_3_0:
				case XNG_API_GL_4_5:
					g_renderer = std::unique_ptr<graphics::renderer>(new gl::forward_renderer);
					return g_renderer->init(g_apiContext.get());

				case XNG_API_DIRECTX_11:
					g_renderer = std::unique_ptr<graphics::renderer>(new dx11::forward_renderer);
					return g_renderer->init(g_apiContext.get());
				default:
					return false;
				}
			}

			void initialize_resource_managers(void)
			{
				res::resource_factory * f = new res::resource_factory;

				f->register_manager(new graphics::mesh_manager);
			}

			bool initialize_api(os::native_window_handle handle, xng_api_version api, bool debug)
			{
				// Release previous

				g_renderer.reset();
				g_apiContext.reset();

				// Initialize new API and compatible renderer

				bool initialized = false;
				
				switch (api)
				{

#ifdef XNG_GL
				case XNG_API_GL_ES_2_0:
				case XNG_API_GL_ES_3_0:
				case XNG_API_GL_4_5:

					g_apiContext = std::unique_ptr<graphics::api_context>(new gl::wgl_api_context);

					if (g_apiContext->init(handle, api, m_debug))
					{
						g_apiContext->use();
						m_apiVersion = (const char*)XNG_GL_RETURN_CHECK(glGetString(GL_VERSION));
						initialized = true;
					}

					break;
#endif

#ifdef XNG_DX11
				case XNG_API_DIRECTX_11:

					g_apiContext = std::unique_ptr<graphics::api_context>(new dx11::dx11_api_context);

					if (g_apiContext->init(handle, api, m_debug))
					{
						dx11::dx11_api_context * ctx = dynamic_cast<dx11::dx11_api_context*>(g_apiContext.get());
						D3D_FEATURE_LEVEL featureLevel = ctx->get_device()->GetFeatureLevel();
						switch (featureLevel)
						{
						case D3D_FEATURE_LEVEL_11_0:
							m_apiVersion = "DirectX 11.0";
							break;
						case D3D_FEATURE_LEVEL_11_1:
							m_apiVersion = "DirectX 11.1";
							break;
						default:
							m_apiVersion = "DirectX 11";
						}
						initialized = true;
					}

					break;
#endif

				}

				if (!initialized)
				{
					g_renderer.reset();
					g_apiContext.reset();
				}
				else
				{
					wxSize size = m_editor->get_render_panel()->GetClientSize();
					g_apiContext->on_resize(size.x, size.y);
					m_currentAPI = api;
					g_apiContext->use();
				}

				return initialized;
			}

			void set_render_loop_status(bool on)
			{
				if (on)
				{
					m_renderMutex.unlock();
					m_timer.Start(10);
				}
				else
				{
					m_renderMutex.lock();
					m_timer.Stop();
				}
			}

			class render_timer : public wxTimer
			{

			public:

				render_timer(void) : wxTimer() {}
				render_timer(app * app) : m_app(app) {}
				void Notify() { m_app->render(); }

			private:

				app * m_app;

			};


			void render(void)
			{
				std::lock_guard<std::mutex> lock(m_renderMutex);

				g_apiContext->frame_start();
				g_renderer->render(g_scene.get(), nullptr);
				g_apiContext->frame_complete();
			}

			void update_title(void)
			{
				wxString title = "XNG Editor";

				if (g_apiContext && g_apiContext->is_initialized())
				{
					title += " [ Graphics API: ";
					title += m_apiVersion;
					title += " ]";
				}
				else
				{
					title += " [ Graphics API: Uninitialized ]";
				}

				m_editor->SetTitle(title);
			}

			void create_menu(void)
			{
				wxMenuBar * menu   = new wxMenuBar;
				wxMenu    * file   = new wxMenu;
				wxMenu    * render = new wxMenu;
				wxMenu    * api    = new wxMenu;

				menu->Append(file, "File");
				menu->Append(render, "Render");
				render->AppendSubMenu(api, "API");

				enum {
					IDM_FILE_EXIT,
					IDM_API_DEBUG, IDM_API_DX11, IDM_API_GL45
				};

				file->Append(IDM_FILE_EXIT, "Exit", nullptr);

				api->AppendRadioItem(IDM_API_DX11, "DirectX 11")->Check(m_currentAPI == XNG_API_DIRECTX_11);
				api->AppendRadioItem(IDM_API_GL45, "OpenGL 4.5")->Check(m_currentAPI == XNG_API_GL_4_5);
				api->AppendCheckItem(IDM_API_DEBUG, "Use debug API")->Check(m_debug);

				menu->Bind(wxEVT_MENU, [=](wxCommandEvent & evt)
				{
					switch (evt.GetId())
					{
					case IDM_FILE_EXIT:
						exit();
						break;
					case IDM_API_DEBUG:
						m_debug = evt.IsChecked();
						initialize_api(m_panel->GetHandle(), m_currentAPI, m_debug);
						initialize_renderer(m_currentAPI);
						break;
					case IDM_API_DX11:
						initialize_api(m_panel->GetHandle(), XNG_API_DIRECTX_11, m_debug);
						initialize_renderer(XNG_API_DIRECTX_11);
						update_title();
						break;
					case IDM_API_GL45:
						initialize_api(m_panel->GetHandle(), XNG_API_GL_4_5, m_debug);
						initialize_renderer(XNG_API_GL_4_5);
						update_title();
						break;

					}
				});

				m_editor->SetMenuBar(menu);
			}

			void exit(void)
			{
				set_render_loop_status(false);
				Exit();
			}

		private:

			frame        * m_editor;
			render_panel * m_panel;
			render_timer   m_timer;
			bool           m_debug;

			std::unique_ptr<core::logger> m_logger;
			std::unique_ptr<std::basic_ostream<char>> m_logStream;

			std::string     m_apiVersion;
			xng_api_version m_currentAPI;

			std::mutex m_renderMutex;

		};
	}
}

IMPLEMENT_APP(xng::editor::app)