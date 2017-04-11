#include "light_page.hpp"

#include "wx_helpers.hpp"
#include <xng/math.hpp>

using namespace xng::editor;
using namespace xng::graphics;
using namespace xng::math;

light_page::light_page(scene_graph_light * light, wxWindow * parent) :
    wxWindow(parent, wxID_ANY)
{
    wxBoxSizer * box = xng_new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer * luminanceBox = xng_new wxStaticBoxSizer(wxHORIZONTAL, this, _("Luminance"));
    wxStaticBoxSizer * ambientBox  = xng_new wxStaticBoxSizer(wxHORIZONTAL, this, _("Ambient"));

    auto colorPicker = xng_new wx_color_picker(
        std::bind(&scene_graph_light::set_color, light, std::placeholders::_1),
        this, wxID_ANY, light->get_color());

    auto intensitySpin = xng_new wx_spin<float>(
        std::bind(&scene_graph_light::get_intensity, light),
        std::bind(&scene_graph_light::set_intensity, light, std::placeholders::_1),
        this, wxID_ANY, 0.f, FLT_MAX, .1f);

    auto ambientColorPicker = xng_new wx_color_picker(
        std::bind(&scene_graph_light::set_ambient_color, light, std::placeholders::_1),
        this, wxID_ANY, light->get_ambient_color());

    auto ambientIntensitySpin = xng_new wx_spin<float>(
        std::bind(&scene_graph_light::get_ambient_intensity, light),
        std::bind(&scene_graph_light::set_ambient_intensity, light, std::placeholders::_1),
        this, wxID_ANY, 0.f, FLT_MAX, .1f);

    luminanceBox->Add(colorPicker, 1, wxEXPAND | wxALL);
    luminanceBox->Add(intensitySpin, 1, wxEXPAND | wxALL);

    ambientBox->Add(ambientColorPicker, 1, wxEXPAND | wxALL);
    ambientBox->Add(ambientIntensitySpin, 1, wxEXPAND | wxALL);

    box->Add(luminanceBox, 0, wxEXPAND | wxALL);
    box->Add(ambientBox, 0, wxEXPAND | wxALL);

    SetSizer(box);
}