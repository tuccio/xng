#pragma once

#include <xng/core.hpp>
#include <xng/res/resource_loader.hpp>
#include <xng/res/resource_ptr.hpp>

#include <cassert>
#include <string>

#include <mutex>

#include <boost/property_tree/ptree.hpp>

enum xng_resource_status
{
	XNG_RESOURCE_NOT_LOADED,
	XNG_RESOURCE_LOADING,
	XNG_RESOURCE_LOADED,
	XNG_RESOURCE_UNLOADING
};

namespace xng
{
	namespace res
	{
		class resource_manager;

		class resource
		{

		public:

			using id_type = uint32_t;
			using parameters_type = boost::property_tree::basic_ptree<std::string, std::string>;

			resource(void);

			resource(const char * name,
				parameters_type params = parameters_type(),
				std::shared_ptr<resource_loader> loader = std::shared_ptr<resource_loader>(),
				resource_manager * owner = nullptr);

			resource(const resource &) = delete;
			resource(resource &&);

			virtual ~resource(void) {}

			resource_manager * get_owner(void) const;
			const char * get_name(void) const;
			xng_resource_status get_status(void) const;

			id_type get_id(void) const;
			size_t get_size(void) const;

			const parameters_type & get_parameters(void) const;

			void lock(void);
			void unlock(void);
			bool try_lock(void);

			bool load(void);

		protected:

			virtual bool load_impl(void) = 0;
			virtual void unload_impl(void) = 0;
			virtual size_t calculate_size_impl(void) = 0;

			void recalculate_size(void);

		private:

			resource_manager * m_owner;

			xng_resource_status m_status;
			id_type m_id;
			size_t m_size;
			std::string m_name;

			std::shared_ptr<resource_loader> m_loader;

			mutable uint32_t   m_references;
			mutable std::mutex m_mutex;

			parameters_type m_parameters;

			void recalculate_size_internal(void);

			/* Calls for resource_manager */

			friend class resource_manager;

			template <typename T>
			friend class resource_ptr;

			void set_id(id_type id);
			void set_owner(resource_manager * owner);
			void set_name(const char * name);

			bool unload(void);
			void unload_internal(void);

			void ref(void);
			void unref(void);
		
		};

		inline resource::parameters_type default_parameters(void) { return resource::parameters_type(); }

	}
}