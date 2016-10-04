#pragma once

#include <xng/os.hpp>
#include <xng/gui.hpp>

#include "wx.hpp"

#include <wx/aui/aui.h>

#include "fps_camera_controller.hpp"
#include "scene_graph_page.hpp"
#include "rendering_settings_page.hpp"

namespace xng
{
	namespace editor
	{
		class editor
		{

		public:

			editor(os::native_window * window);
			~editor(void);

			void update(float dt);

			wxFrame * get_main_window(void);

			bool load_scene(const char * name);

		private:

			void create_gui(void);
			void create_menu(void);

			os::native_window * m_window;
			wxFrame           * m_editor;

			std::unique_ptr<wxAuiManager> m_auiManager;

			std::unique_ptr<os::native_window_observer> m_observer;

			fps_camera_controller m_cameraController;

			gui::window       * m_sunSettings;
			gui::window       * m_cameraSettings;
			gui::text_control * m_fpsText;

			scene_graph_page        * m_sceneGraphPage;
			rendering_settings_page * m_renderingSettingsPage;

		};
	}
}