#pragma once

#include <algorithm>
#include <vector>

namespace xng
{
	namespace core
	{
		template <typename T>
		struct observable
		{

		public:

			template <typename Callback, typename ... Args>
			inline void notify(Callback callback, Args && ... args)
			{
				for (T * observer : m_observers)
				{
					(observer->*callback)(std::forward<Args>(args) ...);
				}
			}

			void add_observer(T * observer)
			{
				m_observers.push_back(observer);
			}

			void remove_observer(T * observer)
			{
				auto it = std::find(m_observers.begin(), m_observers.end(), observer);
				m_observers.erase(it);
			}

		protected:

			void clear_observers(void)
			{
				m_observers.clear();
			}

		private:

			std::vector<T*> m_observers;

		};
	}
}