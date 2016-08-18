#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

#include <xng/core.hpp>

#include <xng/res/resource_manager.hpp>

namespace xng
{
	namespace res
	{
		class resource_factory :
			public core::singleton<resource_factory>
		{

		public:

			resource_factory(void);
			~resource_factory(void);

			void clear(void);

			void register_manager(resource_manager * manager);
			void unregister_manager(const char * type);

			template <typename Resource = resource>
			resource_ptr<Resource> create(const char * type,
				const char * name,
				const resource::parameters_type & parameters = resource::parameters_type(),
				std::shared_ptr<resource_loader> loader = std::shared_ptr<resource_loader>()) const
			{
				resource_ptr<Resource> r;
				std::lock_guard<std::mutex> lock(m_mutex);

				auto it = m_managers.find(type);

				if (it != m_managers.end())
				{
					r = static_pointer_cast<Resource>(it->second->create(name, parameters, loader));
				}
				else
				{
					XNG_LOG_DEBUG(XNG_LOG_STREAM() << "Requested resource type (" << type << ") is unregistered.");
					r = resource_ptr<Resource>();
				}

				return r;
			}

		private:

			std::unordered_map<std::string, resource_manager*> m_managers;
			mutable std::mutex m_mutex;

		};
	}

}