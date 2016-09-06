#pragma once

#include <xng/graphics/mesh.hpp>
#include <xng/res.hpp>

namespace xng
{
	namespace graphics
	{
		class mesh_manager :
			public res::resource_manager
		{

		public:

			mesh_manager(void) :
				res::resource_manager("mesh") {}

		protected:

			res::resource * create_impl(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader) override
			{
				return xng_new mesh(name, params, loader, this);
			}

			void free_impl(res::resource * resource) override
			{
				xng_delete resource;
			}

		};
	}
}