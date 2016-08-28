#pragma once

#include <xng/dx11/gpu_mesh.hpp>
#include <xng/res.hpp>

namespace xng
{
	namespace dx11
	{
		class gpu_mesh_manager :
			public res::resource_manager
		{

		public:

			gpu_mesh_manager(ID3D11Device * device) :
				m_device(device), res::resource_manager("dx11mesh") {}

			XNG_INLINE ID3D11Device * get_device(void) const
			{
				return m_device.get();
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

			com_ptr<ID3D11Device> m_device;

		};
	}
}