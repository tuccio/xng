#pragma once

#include <xng/gl/gpu_mesh.hpp>
#include <xng/res.hpp>

namespace xng
{
	namespace gl
	{
		class gpu_mesh_manager :
			public res::resource_manager
		{

		public:

			gpu_mesh_manager(void) :
				res::resource_manager("glmesh") {}

		protected:

			res::resource * create_impl(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader) override
			{
				return new gpu_mesh(name, params, loader, this);
			}

			void free_impl(res::resource * resource) override
			{
				delete resource;
			}

		};
	}
}