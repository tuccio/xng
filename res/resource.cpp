#include <xng/res/resource.hpp>
#include <xng/res/resource_manager.hpp>

using namespace xng::res;

resource::resource(void) :
	m_owner(nullptr), m_loader(nullptr), m_size(0), m_status(XNG_RESOURCE_NOT_LOADED), m_references(0) {}

resource::resource(const char * name, const typename resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	m_name(name), m_owner(owner), m_loader(loader), m_size(0), m_status(XNG_RESOURCE_NOT_LOADED), m_references(0) {}

resource::resource(resource && rhs) :
	resource()
{
	std::lock_guard<std::mutex> lock(rhs.m_mutex);

	std::swap(m_name,       rhs.m_name);
	std::swap(m_owner,      rhs.m_owner);
	std::swap(m_loader,     rhs.m_loader);
	std::swap(m_size,       rhs.m_size);
	std::swap(m_status,     rhs.m_status);
	std::swap(m_references, rhs.m_references);
}

const typename resource_parameters & resource::get_parameters(void) const
{
	return m_parameters;
}

resource_manager * resource::get_owner(void) const
{
	return m_owner;
}

void resource::set_owner(resource_manager * owner)
{
	m_owner = owner;
}

const char * resource::get_name(void) const
{
	return m_name.c_str();
}

void resource::set_name(const char * name)
{
	m_name = name;
}

xng_resource_status resource::get_status(void) const
{
	return m_status;
}

typename resource_id resource::get_id(void) const
{
	return m_id;
}

void resource::set_id(typename resource_id id)
{
	m_id = id;
}

size_t resource::get_size(void) const
{
	return m_size;
}

void resource::recalculate_size(void)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	recalculate_size_internal();
}

void resource::recalculate_size_internal(void)
{
	size_t previous = m_size;
	m_size = m_status == XNG_RESOURCE_NOT_LOADED ? 0 : calculate_size_impl();

	if (m_owner)
	{
		m_owner->update_resource_space(previous, m_size);
	}
}

void resource::ref(void)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	++m_references;
}

void resource::unref(void)
{
	lock();
	if (--m_references == 0)
	{
		unlock();
		m_owner->collect_resource(this);
	}
	else
	{
		unlock();
	}
}

bool resource::load(void)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_status == XNG_RESOURCE_NOT_LOADED)
	{
		if ((m_loader && m_loader->load(this)) ||
			(!m_loader && load_impl()))
		{
			m_status = XNG_RESOURCE_LOADED;
			recalculate_size_internal();
		}
		else
		{
			XNG_LOG("Resource", XNG_LOG_STREAM() << "Failed to load resource \"" << m_name << "\".");
		}
	}

	return m_status == XNG_RESOURCE_LOADED;
}

bool resource::unload(void)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_references == 0)
	{
		unload_internal();
	}

	return m_status == XNG_RESOURCE_NOT_LOADED;
}

void resource::unload_internal(void)
{
	if (m_status == XNG_RESOURCE_LOADED)
	{
		if (m_loader)
		{
			m_loader->unload(this);
		}
		else
		{
			unload_impl();
		}

		m_status = XNG_RESOURCE_NOT_LOADED;
		recalculate_size_internal();
	}
}

void resource::lock(void)
{
	m_mutex.lock();
}

void resource::unlock(void)
{
	m_mutex.unlock();
}

bool resource::try_lock(void)
{
	return m_mutex.try_lock();
}