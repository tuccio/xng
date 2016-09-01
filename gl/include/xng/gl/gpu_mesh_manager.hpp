#pragma once

#include <xng/gl/gpu_mesh.hpp>
#include <xng/gl/gl_api_context.hpp>
#include <xng/res.hpp>

namespace xng
{
	namespace gl
	{
		class gpu_mesh_manager :
			public res::resource_manager
		{

		public:

			gpu_mesh_manager(gl_api_context * context) :
				res::resource_manager("glmesh"),
				m_context(context) {}

			XNG_INLINE gl_api_context * get_context(void)
			{
				return m_context;
			}

			void garbage_collection(void) override
			{
				// TODO: Should use a shared context to run in a different thread than the rendering thread
				m_context->use();
				resource_manager::garbage_collection();
				m_context->dispose();
			}

		protected:

			res::resource * create_impl(const char * name, const res::resource_parameters & params, res::resource_loader_ptr loader) override
			{
				return new gpu_mesh(name, params, loader, this);
			}

			void free_impl(res::resource * resource) override
			{
				delete resource;
			}

		private:

			gl_api_context * m_context;

		};
	}
}