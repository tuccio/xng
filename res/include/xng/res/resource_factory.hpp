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
            resource_manager * unregister_manager(const char * type);

            bool is_registered(const char * type) const;
            resource_manager * find_manager_by_type(const char * type);


            template <typename Resource = resource, typename Dependency = resource>
            resource_ptr<Resource> create(const char * type,
                const char * name,
                const resource_parameters & parameters = resource_parameters(),
                resource_loader_ptr loader = resource_loader_ptr(),
                resource_ptr<Dependency> dependency = resource_ptr<Dependency>()) const
            {
                resource_ptr<Resource> r;
                std::lock_guard<std::mutex> lock(m_mutex);

                auto it = m_managers.find(type);

                if (it != m_managers.end())
                {
                    r = xng::core::static_pointer_cast<Resource>(
                        it->second->create(
                            name,
                            parameters,
                            loader,
                            xng::core::static_pointer_cast<resource>(dependency)));
                }
                else
                {
                    XNG_LOG_DEBUG(XNG_LOG_STREAM() << "Requested resource type (" << type << ") is unregistered.");
                    r = resource_ptr<Resource>();
                }

                return r;
            }

            template <typename Resource, typename Dependency = resource>
            resource_ptr<Resource> create(
                const char * name,
                const resource_parameters & parameters = resource_parameters(),
                resource_loader_ptr loader = resource_loader_ptr(),
                resource_ptr<Dependency> dependency = resource_ptr<Dependency>()) const
            {
                return create<Resource>(Resource::resource_type, name, parameters, loader, dependency);
            }

            template <typename Resource = resource>
            resource_ptr<Resource> find_by_id(const char * type,
                resource_id id) const
            {
                resource_ptr<Resource> r;
                std::lock_guard<std::mutex> lock(m_mutex);

                auto it = m_managers.find(type);

                if (it != m_managers.end())
                {
                    r = xng::core::static_pointer_cast<Resource>(it->second->find_by_id(id));
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