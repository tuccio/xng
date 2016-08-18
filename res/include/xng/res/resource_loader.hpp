#pragma once

#include <functional>

namespace xng
{
	namespace res
	{

		class resource;

		class resource_loader
		{

		public:

			virtual ~resource_loader(void) = default;
			virtual bool load(resource *) = 0;
			virtual void unload(resource *) = 0;

		};

		class dynamic_resource_loader :
			public resource_loader
		{

		public:

			dynamic_resource_loader(void) = default;

			template <typename Loader, typename Unloader>
			dynamic_resource_loader(Loader && loader, Unloader && unloader) :
				m_loader(loader),
				m_unloader(loader) { }

		protected:

			bool load(resource * r) override { assert(m_loader && m_unloader && "Cannot use dynamic_resource_loader without proper initialization"); return m_loader(r); }
			void unload(resource * r) override { assert(m_unloader && m_unloader && "Cannot use dynamic_resource_loader without proper initialization"); m_unloader(r); }

		private:

			std::function<bool(resource *)> m_loader;
			std::function<void(resource *)> m_unloader;

		};

	}
}