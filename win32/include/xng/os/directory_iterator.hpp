#include <xng/os/path.hpp>

#include <memory>
#include <iterator>
#include <vector>

namespace xng
{
	namespace os
	{
		struct directory_iterator :
			std::iterator<std::input_iterator_tag, path>
		{

			directory_iterator(void);
			directory_iterator(const path & p);
			directory_iterator(const directory_iterator &) = default;
			~directory_iterator(void);

			bool operator== (const directory_iterator &);
			bool operator!= (const directory_iterator &);

			const path & operator* (void) const;
			const path & operator-> (void) const;

			directory_iterator & operator= (const directory_iterator &) = default;

			directory_iterator & operator++(void);
			directory_iterator operator++(int);

		private:

			std::shared_ptr<std::vector<path>> m_paths;
			std::vector<path>::const_iterator  m_iterator;

		};
	}
}