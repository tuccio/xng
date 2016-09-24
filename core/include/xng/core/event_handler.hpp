#pragma once

#include <unordered_map>
#include <xng/core/debug_new.hpp>

namespace xng
{
	namespace core
	{
		template <typename Event, Event evt>
		struct event_callback {};

		template <typename Source, typename Event>
		class event_handler
		{

		public:

			~event_handler(void)
			{
				for (auto & p : m_sourceCallbacksMap)
				{
					for (auto & cb : p.second)
					{
						xng_delete cb.second;
					}
				}
			}

			template <Event evt, typename ... Args>
			void notify_event(Source * source, Args && ... args)
			{
				auto it = m_sourceCallbacksMap.find(source);

				if (it != m_sourceCallbacksMap.end())
				{
					auto eqr = it->second.equal_range(evt);

					for (auto cbIt = eqr.first; cbIt != eqr.second; ++cbIt)
					{
						(*(event_callback<Event, evt>::type*)cbIt->second)(source, std::forward<Args>(args) ...);
					}
				}				
			}

			template <Event evt, typename F>
			void bind(Source * source, F && function)
			{
				m_sourceCallbacksMap[source].emplace(evt, xng_new event_callback<Event, evt>::type(std::forward<F>(function)));
			}

		private:

			typedef std::unordered_multimap<Event, void*>           callbacks_multimap;
			typedef std::unordered_map<Source*, callbacks_multimap> source_map;
			source_map m_sourceCallbacksMap;

		};
	}
}