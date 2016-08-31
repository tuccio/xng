#include <xng/os/path.hpp>

namespace xng
{
	namespace os
	{
		bool exists(const path & p);

		bool is_directory(const path & p);
		bool is_regular_file(const path & p);

		path current_directory(void);
		void change_current_directory(const path & p);
	}
}