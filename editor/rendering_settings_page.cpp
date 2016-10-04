#pragma once

#include "rendering_settings_page.hpp"

#include <xng/graphics.hpp>
#include <xng/engine.hpp>

using namespace xng::editor;
using namespace xng::graphics;
using namespace xng::engine;

rendering_settings_page::rendering_settings_page(wxWindow * parent) :
	wxWindow(parent, wxID_ANY)
{
	const int padding = 5;

	wxStaticBoxSizer * forwardBox = new wxStaticBoxSizer(wxVERTICAL, this, _("Forward Rendering"));

	forwardBox->Add(new wx_checkbox(
		[&](bool value)
	{
		game::get_singleton()->get_render_module()->configuration().set_forward_depth_prepass(value);
	},
		forwardBox->GetStaticBox(), wxID_ANY, _("Depth prepass"), game::get_singleton()->get_render_module()->configuration().get_forward_depth_prepass()), 0, wxEXPAND | wxALL, padding);

	SetSizerAndFit(forwardBox);
}
