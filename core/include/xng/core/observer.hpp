#pragma once

#include <algorithm>
#include <vector>
#include <type_traits>

namespace xng
{
	namespace core
	{
		template <typename T>
		struct observable
		{

		public:

			// Notify all

			template <typename Callback, typename ... Args>
			XNG_INLINE std::enable_if_t<std::is_same<void, std::result_of_t<Callback(T, Args...)>>::value>
				notify(Callback callback, Args && ... args)
			{
				for (T * observer : m_observers)
				{
					(observer->*callback)(std::forward<Args>(args) ...);
				}
			}

			// Notify all with propagation (stops propagating if the return value is 0, or false depending on the return type)

			template <typename Callback, typename ... Args>
			XNG_INLINE std::enable_if_t<!std::is_same<void, std::result_of_t<Callback(T, Args...)>>::value>
				notify(Callback callback, Args && ... args)
			{
				for (T * observer : m_observers)
				{
					if (!(observer->*callback)(std::forward<Args>(args) ...))
					{
						return;
					}
				}
			}

			// Manage observers

			void add_observer(T * observer) const
			{
				m_observers.push_back(observer);
			}

			void remove_observer(T * observer) const
			{
				auto it = std::find(m_observers.begin(), m_observers.end(), observer);
				m_observers.erase(it);
			}

		protected:

			void clear_observers(void) const
			{
				m_observers.clear();
			}

		private:

			mutable std::vector<T*> m_observers;

		};


	}
}