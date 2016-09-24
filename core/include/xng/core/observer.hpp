#pragma once

#include <algorithm>
#include <vector>
#include <type_traits>

namespace xng
{
	namespace core
	{
		struct dummy_mutex
		{
			XNG_INLINE void lock(void) {}
			XNG_INLINE void unlock(void) {}
			XNG_INLINE bool try_lock(void) { return true; }
		};

		template <typename T, typename Mutex = dummy_mutex>
		struct observable
		{

		public:

			// Notify all

			template <typename Callback, typename ... Args>
			XNG_INLINE std::enable_if_t<std::is_same<void, std::result_of_t<Callback(T, Args...)>>::value>
				notify(Callback callback, Args && ... args)
			{
				std::lock_guard<Mutex> lock(m_mutex);

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
				std::lock_guard<Mutex> lock(m_mutex);

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
				std::lock_guard<Mutex> lock(m_mutex);
				m_observers.push_back(observer);
			}

			void remove_observer(T * observer) const
			{
				std::lock_guard<Mutex> lock(m_mutex);
				auto it = std::find(m_observers.begin(), m_observers.end(), observer);
				m_observers.erase(it);
			}

		protected:

			void clear_observers(void) const
			{
				std::lock_guard<Mutex> lock(m_mutex);
				m_observers.clear();
			}

		private:

			mutable std::vector<T*> m_observers;
			mutable Mutex           m_mutex;

		};


	}
}