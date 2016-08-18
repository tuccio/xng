#include <xng/res/resource_manager.hpp>
#include <unordered_set>

using namespace xng::res;

resource_manager::resource_manager(const char * type, size_t spaceThreshold) :
	m_type(type), m_lastID(0), m_spaceThreshold(spaceThreshold), m_usedSpace(0) {}

resource_ptr<resource> resource_manager::create(
	const char * name,
	const resource::parameters_type & parameters,
	std::shared_ptr<resource_loader> loader)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	resource_ptr<resource> r;

	if (name[0] == '\0' || !(r = find_by_name_internal(name)))
	{
		std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
		garbage_collection_internal();

		r = create_impl(name, loader);

		resource::id_type id = ++m_lastID;
		r->set_id(id);
		r->set_owner(this);

		if (name[0] != '\0')
		{
			m_namedResources[name] = r.get();
		}

		m_resources[id] = r.get();
	}

	return r;
} 

void resource_manager::garbage_collection(void)
{
	std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
	std::lock_guard<std::mutex> lock(m_mutex);
	garbage_collection_internal();
}

resource_ptr<resource> resource_manager::find_by_name(const char * name) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return find_by_name_internal(name);
}

resource_ptr<resource> resource_manager::find_by_id(resource::id_type id) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return find_by_id_internal(id);
}

resource_ptr<resource> resource_manager::find_by_name_internal(const char * name) const
{
	resource_ptr<resource> r;
	auto it = m_namedResources.find(name);

	if (it != m_namedResources.end())
	{
		r = it->second;
	}

	return r;
}

resource_ptr<resource> resource_manager::find_by_id_internal(resource::id_type id) const
{
	resource_ptr<resource> r;
	auto it = m_resources.find(id);

	if (it != m_resources.end())
	{
		r = it->second;
	}

	return r;
}

const char * resource_manager::get_type(void) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_type.c_str();
}

size_t resource_manager::get_space_threshold(void) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_spaceThreshold;
}

void resource_manager::set_space_threshold(size_t spaceThreshold)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_spaceThreshold = spaceThreshold;
}

/* Garbage collection */

void resource_manager::collect_resource(resource * r)
{
	std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
	m_garbage.push_front(r);
}

void resource_manager::garbage_collection_internal(void)
{
	garbage_clear_duplicates_internal();

	while (m_usedSpace > m_spaceThreshold && !m_garbage.empty())
	{
		resource * r = m_garbage.back();
		m_garbage.pop_back();

		if (r->unload())
		{
			free_resource_internal(r);
		}
	}

	XNG_LOG_IF(m_usedSpace > m_spaceThreshold,
		XNG_LOG_STREAM() <<
			"Unable to free up enough memory to respect the space constraint for resource type \"" << 
			m_type << "\". Current space usage: (" << m_usedSpace << "/" << m_spaceThreshold << ").");
}

void resource_manager::garbage_clear_duplicates_internal(void)
{
	std::unordered_set<resource*> garbage;

	for (auto it = m_garbage.begin(); it != m_garbage.end();)
	{
		if (garbage.find(*it) != garbage.end())
		{
			it = m_garbage.erase(it);
		}
		else
		{
			garbage.insert(*it);
			++it;
		}
	}
}

void resource_manager::update_resource_space(size_t previous, size_t current)
{
	m_usedSpace = (m_usedSpace + current) - previous;
}

void resource_manager::free_resource_internal(resource * r)
{
	auto itID   = m_resources.find(r->get_id());
	auto itName = m_namedResources.find(r->get_name());

	m_resources.erase(itID);

	if (itName != m_namedResources.end())
	{
		m_namedResources.erase(itName);
	}

	free_impl(r);
}