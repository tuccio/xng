#pragma once


#include "wx.hpp"

#include <wx/spinctrl.h>
#include <wx/clrpicker.h>

#include <functional>
#include <type_traits>

#define XNG_WX_SLIDER_MAX 100000
#define XNG_WX_SLIDER_MIN 0

#include <xng/math.hpp>

namespace xng
{
    namespace editor
    {
        namespace detail
        {
            using namespace xng::math;

            template <typename T, typename Enable = void>
            struct wx_slider_normalizer;

            template <typename T>
            struct wx_slider_normalizer<T, std::enable_if_t<std::is_floating_point<T>::value>>
            {

                wx_slider_normalizer(T min, T max) :
                    m_min(min), m_max(max) {}

            protected:

                inline T to_type(int value) const
                {
                    return m_min + value * (m_max - m_min) / XNG_WX_SLIDER_MAX;
                }

                inline int to_int(T value) const
                {
                    return XNG_WX_SLIDER_MAX * ((value - m_min) / (m_max - m_min));
                }

                inline int get_wx_min(void) const
                {
                    return XNG_WX_SLIDER_MIN;
                }

                inline int get_wx_max(void) const
                {
                    return XNG_WX_SLIDER_MAX;
                }

            private:

                T m_min;
                T m_max;

            };

            template <typename T>
            struct wx_slider_normalizer<T, std::enable_if_t<std::is_integral<T>::value>>
            {

                wx_slider_normalizer(T min, T max) :
                    m_min(min), m_max(max) {}

            protected:

                inline T to_type(int value) const
                {
                    return static_cast<T>(value);
                }

                inline int to_int(T value) const
                {
                    return static_cast<int>(value);
                }

                inline int get_wx_min(void) const
                {
                    return m_min;
                }

                inline int get_wx_max(void) const
                {
                    return m_max;
                }

            private:

                T m_min;
                T m_max;

            };

        }

        template <typename T>
        class wx_slider :
            detail::wx_slider_normalizer<T>,
            public wxSlider
        {

        public:

            template <typename Functor>
            wx_slider(Functor cb, wxWindow * parent, int id, T value, T min, T max) :
                wx_slider_normalizer(min, max),
                wxSlider(parent, id, to_int(value), get_wx_min(), get_wx_max())
            {
                Bind(wxEVT_SLIDER, [=](wxCommandEvent & event) { cb(get_value()); });
            }

            inline T get_value(void) const
            {
                return to_type(GetValue());
            }

            inline void set_value(T value)
            {
                SetValue(to_int(value));
            }

        private:

            T m_min;
            T m_max;

        };

        template <typename T, typename Enable = void>
        class wx_spin;

        template <typename T>
        class wx_spin<T/*, std::enable_if_t<std::is_floating_point<T>::value>*/> :
            public wxSpinCtrlDouble
        {

        public:

            template <typename Setter>
            wx_spin(Setter set, wxWindow * parent, int id, T value, T min, T max, T step = T(0.01)) :
                wxSpinCtrlDouble(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER)
            {
                if (std::is_integral<T>::value) SetDigits(0);

                SetRange(static_cast<double>(min), static_cast<double>(max));
                SetValue(static_cast<double>(value));
                SetIncrement(static_cast<double>(step));

                Bind(wxEVT_SPINCTRLDOUBLE, [=](wxSpinDoubleEvent & event) { set(static_cast<T>(event.GetValue())); });
                Bind(wxEVT_TEXT_ENTER, [=](wxCommandEvent & event)
                {
                    wxString s = event.GetString();
                    double v;
                    if (s.ToCDouble(&v))
                    {
                        set(static_cast<T>(v));
                    }
                });
            }

            template <typename Getter, typename Setter>
            wx_spin(Getter get, Setter set, wxWindow * parent, int id, T min, T max, T step = T(0.01)) :
                wxSpinCtrlDouble(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER),
                m_getter(get)
            {
                if (std::is_integral<T>::value) SetDigits(0);

                SetRange(static_cast<double>(min), static_cast<double>(max));
                SetIncrement(static_cast<double>(step));

                update_value();

                Bind(wxEVT_SPINCTRLDOUBLE, [=](wxSpinDoubleEvent & event) { set(static_cast<T>(event.GetValue())); });

                Bind(wxEVT_TEXT_ENTER, [=](wxCommandEvent & event)
                {
                    wxString s = event.GetString();
                    double v;
                    if (s.ToCDouble(&v))
                    {
                        set(static_cast<T>(v));
                    }
                });
            }

            void update_value(void)
            {
                SetValue(static_cast<double>(m_getter()));
            }

        private:

            std::function<T(void)> m_getter;
        };

        class wx_button :
            public wxButton
        {

        public:

            template <typename Functor>
            wx_button(Functor cb, wxWindow * parent, int id, const wxString & label) :
                wxButton(parent, id, label)
            {
                Bind(wxEVT_BUTTON, [=](wxCommandEvent & event) { cb(); });
            }

        };

        class wx_choice :
            public wxChoice
        {

        public:

            template <typename Functor, typename ... Args>
            wx_choice(Functor cb, wxWindow * parent, int id, Args && ... args)
            {
                wxString choices[] = { args ... };
                Create(parent, id, wxDefaultPosition, wxDefaultSize, sizeof(choices) / sizeof(choices[0]), choices);
                Bind(wxEVT_CHOICE, [=](wxCommandEvent & event) { cb(event.GetInt()); });
            }

        };

        class wx_checkbox :
            public wxCheckBox
        {

        public:

            template <typename Functor, typename ... Args>
            wx_checkbox(Functor cb, wxWindow * parent, int id, const wxString & label, bool value) :
                wxCheckBox(parent, id, label)
            {
                Bind(wxEVT_CHECKBOX, [=](wxCommandEvent & event) { cb(GetValue()); });
                SetValue(value);
            }

        };

        class wx_color_picker :
            public wxColourPickerCtrl
        {

        public:

            template <typename Functor>
            wx_color_picker(Functor cb, wxWindow * parent, int id, const xng::math::ubyte4 & color)
            {
                wxColour wx = wxColour(color.x, color.y, color.z, color.w);

                Create(parent, id, wx);

                Bind(wxEVT_COLOURPICKER_CHANGED, [=](wxColourPickerEvent & event)
                {
                    wxColour value = event.GetColour();

                    cb(ubyte4(
                        value.Red(),
                        value.Green(),
                        value.Blue(),
                        value.Alpha()));
                });
            }

            template <typename Functor>
            wx_color_picker(Functor cb, wxWindow * parent, int id, const xng::math::ubyte3 & color)
            {
                wxColour wx = wxColour(color.x, color.y, color.z, 255);

                Create(parent, id, wx);

                Bind(wxEVT_COLOURPICKER_CHANGED, [=](wxColourPickerEvent & event)
                {
                    wxColour value = event.GetColour();

                    cb(xng::math::ubyte3(
                        value.Red(),
                        value.Green(),
                        value.Blue()));
                });
            }

        };
    }
}