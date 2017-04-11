#include <xng/res/resource_factory.hpp>

using namespace xng::res;

resource_factory::resource_factory(void) {}

resource_factory::~resource_factory(void)
{
    clear();
}

void resource_factory::clear(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_managers.begin(); it != m_managers.end(); ++it)
    {
        it->second->clear();
    }

    m_managers.clear();
}

void resource_factory::register_manager(resource_manager * manager)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_managers[manager->get_type()] = manager;
}

resource_manager * resource_factory::unregister_manager(const char * type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_managers.find(type);

    if (it != m_managers.end())
    {
        resource_manager * resourceManager = it->second;
        m_managers.erase(it);
        return resourceManager;
    }

    return nullptr;
}

bool resource_factory::is_registered(const char * type) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_managers.find(type);

    return it != m_managers.end();
}

resource_manager * resource_factory::find_manager_by_type(const char * type)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_managers.find(type);

    return it != m_managers.end() ? it->second : nullptr;
}