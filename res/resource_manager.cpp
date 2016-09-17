#include <xng/res/resource_manager.hpp>
#include <unordered_set>

using namespace xng::res;

resource_manager::resource_manager(const char * type, size_t spaceThreshold) :
	m_type(type), m_lastID(0), m_spaceThreshold(spaceThreshold), m_usedSpace(0) {}

resource_manager::~resource_manager(void)
{
	std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
	std::lock_guard<std::mutex> lock(m_mutex);

	// Detach alive resources
	for (auto & p : m_resources)
	{
		p.second->m_owner = nullptr;
	}
}

resource_ptr<resource> resource_manager::create(
	const char * name,
	const resource_parameters & parameters,
	resource_loader_ptr loader,
	resource_ptr<resource> dependency)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	resource_ptr<resource> r;

	if (!(r = find_by_name_internal(name)) &&
		!(r = find_by_dependency_internal(dependency)))
	{
		std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
		garbage_collection_lazy_internal();

		r = create_impl(name, parameters, loader);

		resource_id id = ++m_lastID;
		r->set_id(id);
		r->set_owner(this);

		if (name[0] != '\0')
		{
			m_namedResources[name] = r.get();
		}

		if (dependency)
		{
			m_dependencies[dependency.get()] = r.get();
			r->set_dependency(dependency.get());
		}

		m_resources[id] = r.get();
	}

	return r;
}

void resource_manager::clear(void)
{
	garbage_collection_full_internal();

#ifdef XNG_DEBUG
	if (m_usedSpace != 0)
	{
		auto stream = XNG_LOG_STREAM();

		stream << "Cleanup incomplete for type \"" << get_type() << "\". Space used is "
			<< m_usedSpace << "/" << m_spaceThreshold << ". Follows a list of objects still alive:" << std::endl;

		for (auto p : m_resources)
		{
			if (p.second->get_name())
			{
				stream << p.second->get_name();
			}
			else
			{
				stream << "<Unnamed object #" << p.second->get_id() << ">";
			}
			stream << std::endl;
		}

		XNG_LOG("Resource Manager", stream);
	}
#endif
}

void resource_manager::garbage_collection(void)
{
	std::lock_guard<std::mutex> garbageLock(m_garbageMutex);
	std::lock_guard<std::mutex> lock(m_mutex);
	garbage_collection_full_internal();
}

resource_ptr<resource> resource_manager::find_by_name(const char * name) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return find_by_name_internal(name);
}

resource_ptr<resource> resource_manager::find_by_id(resource_id id) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return find_by_id_internal(id);
}

resource_ptr<resource> resource_manager::find_by_dependency(resource_ptr<resource> r) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return find_by_dependency_internal(r);
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

resource_ptr<resource> resource_manager::find_by_id_internal(resource_id id) const
{
	resource_ptr<resource> r;
	auto it = m_resources.find(id);

	if (it != m_resources.end())
	{
		r = it->second;
	}

	return r;
}

resource_ptr<resource> resource_manager::find_by_dependency_internal(resource_ptr<resource> dependency) const
{
	resource_ptr<resource> r;
	auto it = m_dependencies.find(dependency.get());

	if (it != m_dependencies.end())
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

size_t resource_manager::get_used_space(void) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_usedSpace;
}

/* Garbage collection */

void resource_manager::collect_resource(resource * r)
{
	std::lock_guard<std::mutex> garbageLock(m_garbageMutex);

	if (m_garbageSet.find(r) == m_garbageSet.end())
	{
		m_garbage.push_front(r);
		m_garbageSet.insert(r);
	}
}

void resource_manager::garbage_collection_lazy_internal(void)
{
	while (m_usedSpace > m_spaceThreshold && !m_garbage.empty())
	{
		resource * r = m_garbage.back();
		m_garbage.pop_back();
		m_garbageSet.erase(m_garbageSet.find(r));

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

void resource_manager::garbage_collection_full_internal(void)
{
	//garbage_clear_duplicates_internal();

	while (!m_garbage.empty())
	{
		resource * r = m_garbage.back();
		m_garbage.pop_back();
		m_garbageSet.erase(m_garbageSet.find(r));

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