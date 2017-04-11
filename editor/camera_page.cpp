#include "camera_page.hpp"

#include "wx_helpers.hpp"
#include <xng/math.hpp>

using namespace xng::editor;
using namespace xng::graphics;
using namespace xng::math;

camera_page::camera_page(scene_graph_camera * cam, wxWindow * parent) :
    wxWindow(parent, wxID_ANY)
{
    wxBoxSizer * box = xng_new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer * nearBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Near plane"));
    wxStaticBoxSizer * farBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Far plane"));
    wxStaticBoxSizer * fovyBox = xng_new wxStaticBoxSizer(wxVERTICAL, this, _("Field of View"));

    camera * c = cam->get_camera();

    auto znearSpin = xng_new wx_spin<float> (
        std::bind(&camera::get_znear, c),
        std::bind(&camera::set_znear, c, std::placeholders::_1),
        this, wxID_ANY, 0.f, FLT_MAX, .001f);

    auto zfarSpin = xng_new wx_spin<float>(
        std::bind(&camera::get_zfar, c),
        std::bind(&camera::set_zfar, c, std::placeholders::_1),
        this, wxID_ANY, 0.f, FLT_MAX, 1.f);

    auto fovySpin = xng_new wx_spin<float>(
        [=]()
    {
        return to_degrees(c->get_fovy());
    },
        [=](float x)
    {
        c->set_fovy(to_radians(x));
    },
        this, wxID_ANY, 0.f, 360, 1.f);

    nearBox->Add(znearSpin, 0, wxEXPAND | wxALL);
    farBox->Add(zfarSpin, 0, wxEXPAND | wxALL);
    fovyBox->Add(fovySpin, 0, wxEXPAND | wxALL);

    box->Add(nearBox, 0, wxEXPAND | wxALL);
    box->Add(farBox, 0, wxEXPAND | wxALL);
    box->Add(fovyBox, 0, wxEXPAND | wxALL);

    SetSizer(box);
}