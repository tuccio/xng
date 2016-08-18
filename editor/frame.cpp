#include "frame.hpp"

#include <fstream>

using namespace xng::editor;

frame::frame(const wxString & caption, wxPoint position, wxSize size) :
	wxFrame(nullptr, wxID_ANY, caption, position, size)
{
	m_renderPanel = new render_panel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);

	m_manager.SetManagedWindow(this);
	m_manager.AddPane(m_renderPanel, wxCENTER);

	m_manager.GetPane(m_renderPanel).name = "render_panel";

	m_manager.Update();
}

frame::~frame(void)
{
	m_manager.UnInit();
}

bool frame::load_aui_conf(const char * filename)
{
	std::wifstream f(filename);

	if (f)
	{
		std::wstring content;
		f >> content;

		wxString perspective = content;
		m_manager.LoadPerspective(perspective);

		return true;
	}

	return false;
}

bool frame::save_aui_conf(const char * filename)
{
	std::wofstream f(filename);

	if (f)
	{
		f << m_manager.SavePerspective().utf8_str().data();
		return true;
	}

	return false;
}