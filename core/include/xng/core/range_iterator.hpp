#pragma once

namespace xng
{
    namespace core
    {
        template <typename Iterator>
        struct range_iterator
        {
            range_iterator(Iterator begin, Iterator end) :
                m_begin(begin), m_end(end) {}

            Iterator begin(void)
            {
                return m_begin;
            }

            Iterator end(void)
            {
                return m_end;
            }

        private:

            Iterator m_begin, m_end;

        };

        template <typename Iterator>
        range_iterator<Iterator> make_range_iterator(Iterator && begin, Iterator && end)
        {
            return range_iterator<Iterator>(std::forward<Iterator>(begin), std::forward<Iterator>(end));
        }
    }
}