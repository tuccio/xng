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

    wxBoxSizer * sizer = xng_new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer * debugBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Debug"));

    debugBox->Add(xng_new wx_checkbox(
        [&](bool value)
    {
        game::get_singleton()->get_render_module()->configuration().set_debug_normals(value);
    },
        debugBox->GetStaticBox(), wxID_ANY, _("Show normals"), game::get_singleton()->get_render_module()->configuration().get_debug_normals()), 0, wxEXPAND | wxALL, padding);

    debugBox->Add(xng_new wx_checkbox(
        [&](bool value)
    {
        game::get_singleton()->get_render_module()->configuration().set_debug_camera_frustum(value);
    },
        debugBox->GetStaticBox(), wxID_ANY, _("Show current camera frustum"), game::get_singleton()->get_render_module()->configuration().get_debug_camera_frustum()), 0, wxEXPAND | wxALL, padding);

    wxStaticBoxSizer * forwardBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Forward Rendering"));

    forwardBox->Add(xng_new wx_checkbox(
        [&](bool value)
    {
        game::get_singleton()->get_render_module()->configuration().set_forward_depth_prepass(value);
    },
        forwardBox->GetStaticBox(), wxID_ANY, _("Depth prepass"), game::get_singleton()->get_render_module()->configuration().get_forward_depth_prepass()), 0, wxEXPAND | wxALL, padding);

    sizer->Add(debugBox, 0, wxEXPAND | wxALL);
    sizer->Add(forwardBox, 0, wxEXPAND | wxALL);

    SetSizerAndFit(sizer);
}
