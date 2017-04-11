#pragma once

namespace xng
{
    namespace core
    {
        template <typename T>
        class refcounted_ptr
        {

        public:

            inline refcounted_ptr(void) : m_pointer(nullptr) {}
            inline refcounted_ptr(const refcounted_ptr<T> & r) : refcounted_ptr(r.get()) {}

            inline refcounted_ptr(T * pointer) : m_pointer(pointer)
            {
                if (m_pointer)
                {
                    m_pointer->ref();
                }
            }

            inline refcounted_ptr(refcounted_ptr<T> && r)
            {
                m_pointer = r.m_pointer;
                r.m_pointer = nullptr;
            }

            inline ~refcounted_ptr(void)
            {
                if (m_pointer)
                {
                    m_pointer->unref();
                }
            }

            inline T * get(void) const
            {
                return m_pointer;
            }

            inline T * const * get_address(void) const
            {
                return &m_pointer;
            }

            inline T ** get_address(void)
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

            inline T * release(void)
            {
                T * ptr = m_pointer;
                m_pointer = nullptr; return ptr;
            }

            inline void swap(refcounted_ptr<T> & r)
            {
                std::swap(m_pointer, r.m_pointer);
            }

            inline operator bool() const
            {
                return m_pointer != nullptr;
            }

            inline T * operator-> (void) const
            {
                return m_pointer;
            }

            inline T * const * operator&(void) const
            {
                return &m_pointer;
            }

            inline T ** operator&(void)
            {
                return &m_pointer;
            }

            inline bool operator == (const refcounted_ptr<T> & r)
            {
                return m_pointer == r.m_pointer;
            }

            inline bool operator != (const refcounted_ptr<T> & r)
            {
                return m_pointer != r.m_pointer;
            }

            inline refcounted_ptr & operator= (T * r)
            {
                reset();
                m_pointer = r;
                m_pointer->ref();
                return *this;
            }

            inline refcounted_ptr & operator= (const refcounted_ptr<T> & r)
            {
                reset();
                m_pointer = r.m_pointer;

                if (m_pointer)
                {
                    m_pointer->ref();
                }

                return *this;
            }

            inline refcounted_ptr & operator= (refcounted_ptr<T> && r)
            {
                reset();
                swap(r);
                return *this;
            }

        private:

            T * m_pointer;

        };

        template <typename U, typename T>
        inline refcounted_ptr<U> static_pointer_cast(const refcounted_ptr<T> & p)
        {
            return refcounted_ptr<U>(static_cast<U*>(p.get()));
        }

        template <typename U, typename T>
        inline refcounted_ptr<U> dynamic_pointer_cast(const refcounted_ptr<T> & p)
        {
            return refcounted_ptr<U>(dynamic_cast<U*>(p.get()));
        }

        template <typename U, typename T>
        inline refcounted_ptr<U> reinterpret_pointer_cast(const refcounted_ptr<T> & p)
        {
            return refcounted_ptr<U>(reinterpret_cast<U*>(p.get()));
        }

    }
}