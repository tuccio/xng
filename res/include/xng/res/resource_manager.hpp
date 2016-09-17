#pragma once

#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include <xng/core.hpp>
#include <xng/res/resource.hpp>

namespace xng
{
	namespace res
	{
		class resource_manager
		{

		public:

			resource_manager(const char * type, size_t spaceThreshold = 1 << 27);
			virtual ~resource_manager(void);

			resource_ptr<resource> create(const char * name,
				const resource_parameters & parameters = resource_parameters(),
				resource_loader_ptr loader = resource_loader_ptr(),
				resource_ptr<resource> dependency = resource_ptr<resource>());

			virtual void garbage_collection(void);

			resource_ptr<resource> find_by_name(const char * name) const;
			resource_ptr<resource> find_by_id(resource_id id) const;
			resource_ptr<resource> find_by_dependency(resource_ptr<resource> r) const;

			size_t get_space_threshold(void) const;
			void set_space_threshold(size_t size);

			size_t get_used_space(void) const;

			const char * get_type(void) const;

			void clear(void);

		protected:

			virtual resource * create_impl(const char * name, const resource_parameters & params, resource_loader_ptr loader) = 0;
			virtual void free_impl(resource * resource) = 0;

		private:

			resource_id  m_lastID;
			size_t       m_spaceThreshold;
			size_t       m_usedSpace;
			std::string  m_type;

			std::unordered_map<std::string, resource*> m_namedResources;
			std::unordered_map<resource_id, resource*> m_resources;
			std::unordered_map<resource*, resource*>   m_dependencies;

			std::list<resource*>          m_garbage;
			std::unordered_set<resource*> m_garbageSet;

			mutable std::mutex m_mutex;
			mutable std::mutex m_garbageMutex;

			/* Non interlocked implementations */

			resource_ptr<resource> find_by_name_internal(const char * name) const;
			resource_ptr<resource> find_by_id_internal(resource_id id) const;
			resource_ptr<resource> find_by_dependency_internal(resource_ptr<resource> r) const;

			/* Resources calls */

			friend class resource;
			friend class resource_factory;

			void collect_resource(resource * r);
			void update_resource_space(size_t previous, size_t current);

			void free_resource_internal(resource * r);
			void garbage_collection_lazy_internal(void);
			void garbage_collection_full_internal(void);
			void garbage_clear_duplicates_internal(void);


		};

		template <typename Resource>
		class basic_resource_manager :
			public resource_manager
		{

		public:

			basic_resource_manager(void) :
				resource_manager(Resource::resource_type) {}

		protected:

			res::resource * create_impl(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader) override
			{
				return xng_new Resource(name, params, loader, this);
			}

			void free_impl(res::resource * resource) override
			{
				xng_delete resource;
			}
		};
	}
}