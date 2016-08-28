#pragma once

#include <xng/dx11/dx11_headers.hpp>

namespace xng
{
	namespace dx11
	{
		class constant_buffer
		{

		public:

			XNG_INLINE bool create(ID3D11Device * device, size_t size, const void * data = nullptr)
			{
				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = data;

				return !XNG_HR_FAILED(device->CreateBuffer(
					&CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
					data ? &initData : nullptr,
					m_buffer.reset_and_get_address()));
			}

			template <typename T>
			XNG_INLINE bool create(ID3D11Device * device, const T * data = nullptr)
			{
				return create(device, sizeof(T), data);
			}

			XNG_INLINE void * map(ID3D11DeviceContext * context)
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				
				if (XNG_HR_FAILED(context->Map(m_buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
				{
					return nullptr;
				}
				else
				{
					return mappedResource.pData;
				}
			}

			XNG_INLINE void unmap(ID3D11DeviceContext * context)
			{
				context->Unmap(m_buffer.get(), 0);
			}

			template <typename T>
			XNG_INLINE bool write(ID3D11DeviceContext * context, const T * data, size_t n = 1)
			{
				void * dest = map(context);

				if (dest)
				{
					memcpy(dest, data, sizeof(T) * n);
					unmap(context);
					return true;
				}

				return false;
			}

			XNG_INLINE ID3D11Buffer * const * operator&(void) const
			{
				return &m_buffer;
			}

			XNG_INLINE ID3D11Buffer * operator->(void) const
			{
				return m_buffer.get();
			}

			XNG_INLINE ID3D11Buffer * get(void) const
			{
				return m_buffer.get();
			}

			XNG_INLINE void reset(void)
			{
				m_buffer.reset();
			}

		private:

			com_ptr<ID3D11Buffer> m_buffer;

		};
	}
}