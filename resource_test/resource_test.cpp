#pragma once

#include <xng/res.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <atomic>

class reverse_name :
	public xng::res::resource
{

public:

	reverse_name(const char * name) :
		resource(name) {}

	const char * get(void) const
	{
		return m_resource->c_str();
	}

protected:

	bool load_impl(void) override
	{
		std::string name = get_name();
		m_resource = new std::string(name.rbegin(), name.rend());
		return true;
	}

	void unload_impl(void) override
	{
		delete m_resource;
	}

	size_t calculate_size_impl(void) override
	{
		return m_resource->size();
	}

private:

	std::string * m_resource;

};

typedef xng::res::resource_ptr<reverse_name> reverse_name_ptr;

class reverse_name_manager :
	public xng::res::resource_manager
{

public:

	reverse_name_manager(void) : resource_manager("reverse_name", 1 << 10) {}

protected:


	xng::res::resource * create_impl(const char * name, std::shared_ptr<xng::res::resource_loader> loader)
	{
		return new reverse_name(name);
	}

	void free_impl(xng::res::resource * resource)
	{
		delete resource;
	}

};

std::atomic<bool> g_success = true;

void worker(void)
{
	const int N = 100000;
	bool success = true;

	for (int i = 0; i < N; ++i)
	{
		std::string objectName = "object" + std::to_string(i);
		std::string reverseName(objectName.rbegin(), objectName.rend());

		reverse_name_ptr r = xng::res::resource_factory::get_singleton()->
			create<reverse_name>("reverse_name", objectName.c_str());

		if (r->load())
		{
			XNG_LOG_IF(reverseName != r->get(), "Assertion failed.");
			
			if (reverseName != r->get())
			{
				success = false;
			}
		}
		else
		{
			XNG_LOG_DEBUG("Unable to load resource.");
		}
	}

	XNG_LOG("Thread Status", success ? "Thread terminated successfully." : "Thread terminated with error.");
	g_success = g_success && success;
}

int main(int argc, char * argv[])
{
	auto logger = std::make_unique<xng::core::logger>(&std::cout);

	xng::res::resource_factory factory;
	std::unique_ptr<reverse_name_manager> reverseNameManager = std::make_unique<reverse_name_manager>();

	factory.register_manager(reverseNameManager.get());

	const int N = 64;

	std::vector<std::thread> threads;

	for (int i = 0; i < N; ++i)
	{
		threads.emplace_back(worker);
	}
	
	for (auto & thread : threads)
	{
		thread.join();
	}

	XNG_LOG("Consistency Test", g_success ? "Successful." : "Failed.");

	factory.unregister_manager(reverseNameManager->get_type());

	if (reverseNameManager->get_used_space() == 0)
	{
		XNG_LOG("Cleanup Test", g_success ? "Successful." : "Failed.");
	}

	return 0;
}