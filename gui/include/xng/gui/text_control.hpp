#pragma once

#include <xng/gui/widget.hpp>

namespace xng
{
    namespace gui
    {
        class text_control :
            public widget
        {

        public:

            text_control(gui_manager * manager, widget * parent, const math::int2 & position = math::int2(0), const math::int2 & size = math::int2(0));

            void extract(gui_command_list_inserter & inserter, const style & style) const override;

            template <typename String>
            void set_text(const String & str)
            {
                core::string_convert(str, m_text);
            }

            template <typename String>
            void set_text_and_fit(const String & str)
            {
                core::string_convert(str, m_text);
                fit();
            }

            const wchar_t * get_text(void) const;

            bool is_editable(void) const;
            void set_editable(bool editable);

            void fit(void);

            void set_text_control_style(const text_control_style & style);

        protected:

            const text_control_style * get_text_control_style(void) const;

        private:

            std::wstring m_text;
            bool         m_editable;

            std::unique_ptr<text_control_style> m_style;

        };
    }
}