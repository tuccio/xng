#pragma once

#include <xng/core.hpp>
#include <xng/res/resource_loader.hpp>

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

		typedef uint32_t resource_id;
		typedef boost::property_tree::basic_ptree<std::string, std::string> resource_parameters;

		class resource
		{

		public:

			resource(void);

			resource(const char * name,
				const resource_parameters & params = resource_parameters(),
				resource_loader_ptr loader = resource_loader_ptr(),
				resource_manager * owner = nullptr);

			resource(const resource &) = delete;
			resource(resource &&);

			virtual ~resource(void) {}

			resource_manager * get_owner(void) const;
			const char * get_name(void) const;
			xng_resource_status get_status(void) const;

			resource_id get_id(void) const;
			size_t get_size(void) const;

			const resource_parameters & get_parameters(void) const;

			void lock(void);
			void unlock(void);
			bool try_lock(void);

			bool load(const void * userdata = nullptr);

		protected:

			virtual bool load_impl(const void * userdata) = 0;
			virtual void unload_impl(void) = 0;
			virtual size_t calculate_size_impl(void) = 0;

			void recalculate_size(void);

			resource * get_dependency(void) const;
			void set_dependency(resource * r);

		private:

			resource_manager    * m_owner;
			resource            * m_dependency;

			xng_resource_status   m_status;
			resource_id           m_id;
			size_t                m_size;
			std::string           m_name;

			resource_loader_ptr   m_loader;

			mutable uint32_t      m_references;
			mutable std::mutex    m_mutex;

			resource_parameters   m_parameters;

			void recalculate_size_internal(void);

			/* Calls for resource_manager */

			friend class resource_manager;

			template <typename Resource>
			friend class xng::core::refcounted_ptr;

			void set_id(resource_id id);
			void set_owner(resource_manager * owner);
			void set_name(const char * name);

			bool unload(void);
			void unload_internal(void);

			void ref(void);
			void unref(void);
		
		};

		inline resource_parameters default_parameters(void) { return resource_parameters(); }

		template <typename Resource>
		using resource_ptr = xng::core::refcounted_ptr<Resource>;

		namespace detail
		{
			template <typename ... Args>
			struct resource_parameters_factory;

			template <typename Arg1, typename Arg2, typename ... Args>
			struct resource_parameters_factory<Arg1, Arg2, Args...>
			{
				XNG_INLINE static void put(resource_parameters & params, Arg1 && arg1, Arg2 && arg2, Args && ... args)
				{
					params.put(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
					resource_parameters_factory<Args...>::put(params, std::forward<Args>(args) ...);
				}
			};

			template <>
			struct resource_parameters_factory<>
			{
				XNG_INLINE static void put(resource_parameters & params) {}
			};
		}

		template <typename ... Args>
		XNG_INLINE resource_parameters make_resource_parameters(Args && ... args)
		{
			static_assert(sizeof ... (Args) % 2 == 0, "make_resource_parameters requires an even number of arguments.");
			resource_parameters params;
			detail::resource_parameters_factory<Args ...>::put(params, std::forward<Args>(args) ...);
			return params;
		}

	}
}