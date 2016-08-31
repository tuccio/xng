#pragma once

#include <cassert>

namespace xng
{
	namespace core
	{
		template <typename T>
		struct XNG_SHARE singleton
		{

			singleton(void)
			{
				assert(!s_singleton && "Cannot create two instances of a singleton class.");
				s_singleton = static_cast<T*>(this);
			}

			singleton(const singleton &) = delete;
			singleton(singleton &&) = default;

			~singleton(void)
			{
				s_singleton = nullptr;
			}

			static T * get_singleton(void)
			{
				return s_singleton;
			}

		private:

			static T * s_singleton;

		};

#ifdef XNG_SHARE_EXPORTER

		template <typename T>
		T * singleton<T>::s_singleton;

#endif

	}
}