#include <xng/engine/module_manager.hpp>
#include <algorithm> 

using namespace xng::engine;
using namespace xng::os;

std::vector<module_factory*> module_manager::get_modules(void) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_modules;
}

module_factory * module_manager::find_module_by_name(const char * name)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = std::find_if(m_modules.begin(), m_modules.end(), [=](module_factory * f)
	{
		return strcmp(name, f->name()) == 0;
	});

	return it == m_modules.end() ? nullptr : *it;
}

module_factory * module_manager::find_module_by_type(xng_module_type type)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = std::find_if(m_modules.begin(), m_modules.end(), [=](module_factory * f)
	{
		return type == f->type();
	});

	return it == m_modules.end() ? nullptr : *it;
}

void module_manager::register_module(module_factory * factory)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_modules.push_back(factory);
}

void module_manager::unregister_module(module_factory * factory)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = std::find(m_modules.begin(), m_modules.end(), factory);

	if (it != m_modules.end())
	{
		auto it2 = m_libraries.find(*it);

		if (it2 != m_libraries.end())
		{
			m_libraries.erase(it2);
		}

		m_modules.erase(it);
	}
}

bool module_manager::register_shared_library(const path & path)
{
	shared_library library;

	if (library.load(path))
	{
		export_modules_t get_exported_modules = (export_modules_t)library.get_symbol_address(XNG_EXPORT_MODULES_FNAME);

		module_factory ** factories = get_exported_modules();

		if (factories)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			XNG_LOG("Loading dynamic module", path.c_str());
			std::shared_ptr<shared_library> persistentLibrary(xng_new shared_library(std::move(library)));

			for (module_factory ** factory = factories; *factory != nullptr; ++factory)
			{
				m_modules.push_back(*factory);
				m_libraries.emplace(*factory, persistentLibrary);
				XNG_LOG("Module loaded", XNG_LOG_STREAM() << (*factory)->name() << " (" << (*factory)->description() << ").");
			}

			return true;
		}
	}

	return false;
}

void module_manager::unregister_shared_library(const path & path)
{

}