#include <xng/os/win32_headers.hpp>
#include <xng/os/directory_iterator.hpp>

#include <cstring>

namespace xng
{
    namespace os
    {
        directory_iterator::directory_iterator(void) {}

        directory_iterator::directory_iterator(const path & p)
        {
            path wildcard = p / path("*");

            WIN32_FIND_DATAW ffd;
            HANDLE hFind = FindFirstFileW(wildcard.c_str(), &ffd);

            if (hFind == INVALID_HANDLE_VALUE)
            {
                m_paths.reset();
            }
            else
            {
                m_paths = std::make_shared<std::vector<path>>();

                do
                {
                    if (wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0)
                    {
                        path currentFile = p / path(ffd.cFileName);
                        m_paths->emplace_back(std::move(currentFile.make_preferred()));
                    }
                } while (FindNextFileW(hFind, &ffd) != 0);

                m_iterator = m_paths->begin();
                FindClose(hFind);
            }
        }

        directory_iterator::~directory_iterator(void)
        {
            m_paths.reset();
        }

        bool directory_iterator::operator== (const directory_iterator & rhs)
        {
            return (m_paths == rhs.m_paths && m_iterator == rhs.m_iterator) ||
                (m_paths && !rhs.m_paths && m_iterator == m_paths->end()) ||
                (!m_paths && rhs.m_paths && rhs.m_iterator == rhs.m_paths->end());
        }

        bool directory_iterator::operator!= (const directory_iterator & rhs)
        {
            return !(*this == rhs);
        }

        const path & directory_iterator::operator* (void) const
        {
            return *m_iterator;
        }

        const path & directory_iterator::operator-> (void) const
        {
            return *m_iterator;
        }

        directory_iterator & directory_iterator::operator++(void)
        {
            ++m_iterator;
            return *this;
        }

        directory_iterator directory_iterator::operator++(int)
        {
            directory_iterator it = *this;
            ++(*this);
            return it;
        }

    }
}