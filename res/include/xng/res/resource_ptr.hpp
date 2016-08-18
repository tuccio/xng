#pragma once

namespace xng
{
	namespace res
	{

		template <typename Resource>
		class resource_ptr
		{

		public:

			inline resource_ptr(void) : m_pointer(nullptr) {}
			inline resource_ptr(const resource_ptr<Resource> & r) : resource_ptr(r.get()) {}

			inline resource_ptr(Resource * pointer) : m_pointer(pointer)
			{
				if (m_pointer)
				{
					m_pointer->ref();
				}
			}

			inline resource_ptr(resource_ptr<Resource> && r)
			{
				m_pointer = r.m_pointer;
				r.m_pointer = nullptr;
			}

			inline ~resource_ptr(void)
			{
				if (m_pointer)
				{
					m_pointer->unref();
				}
			}

			inline Resource * get(void) const
			{
				return m_pointer;
			}

			inline Resource * const * get_address(void) const
			{
				return &m_pointer;
			}

			inline Resource ** get_address(void)
			{
				return &m_pointer;
			}

			inline void reset(void)
			{
				if (m_pointer)
				{
					m_pointer->unref();
					m_pointer = nullptr;
				}
			}

			inline Resource * release(void)
			{
				Resource * ptr = m_pointer;
				m_pointer = nullptr; return ptr;
			}

			inline void swap(resource_ptr<Resource> & r)
			{
				std::swap(m_pointer, r.m_pointer);
			}

			inline operator bool() const
			{
				return m_pointer != nullptr;
			}

			inline Resource * operator-> (void) const
			{
				return m_pointer;
			}

			inline Resource * const * operator&(void) const
			{
				return &m_pointer;
			}

			inline Resource ** operator&(void)
			{
				return &m_pointer;
			}

			inline bool operator == (const resource_ptr<Resource> & r)
			{
				return m_pointer == r.m_pointer;
			}

			inline bool operator != (const resource_ptr<Resource> & r)
			{
				return m_pointer != r.m_pointer;
			}

			inline resource_ptr & operator= (Resource * r)
			{
				reset();
				m_pointer = r;
				m_pointer->ref();
				return *this;
			}

			inline resource_ptr & operator= (const resource_ptr<Resource> & r)
			{
				reset();
				m_pointer = r.m_pointer;
				m_pointer->ref();
				return *this;
			}

			inline resource_ptr & operator= (resource_ptr<Resource> && r)
			{
				reset();
				swap(r);
				return *this;
			}

		private:

			Resource * m_pointer;

		};

		template <typename U, typename T>
		inline resource_ptr<U> static_pointer_cast(const resource_ptr<T> & p)
		{
			return resource_ptr<U>(static_cast<U*>(p.get()));
		}

		template <typename U, typename T>
		inline resource_ptr<U> dynamic_pointer_cast(const resource_ptr<T> & p)
		{
			return resource_ptr<U>(dynamic_cast<U*>(p.get()));
		}

		template <typename U, typename T>
		inline resource_ptr<U> reinterpret_pointer_cast(const resource_ptr<T> & p)
		{
			return resource_ptr<U>(reinterpret_cast<U*>(p.get()));
		}

	}
}