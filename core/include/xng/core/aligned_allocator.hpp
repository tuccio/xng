#pragma once

#include <xng/core/allocator_base.hpp>

#ifdef XNG_WIN32
#define XNG_ALIGNED_ALLOC(Ptr, Size, Alignment) ((Ptr = _aligned_malloc(Size, Alignment)) != nullptr)
#define XNG_ALIGNED_FREE(Ptr) _aligned_free(Ptr)
#elif defined(XNG_POSIX)
#include <cstdlib>
#define XNG_ALIGNED_ALLOC(Ptr, Size, Alignment) (posix_memalign(&Ptr, Alignment, Size) == 0)
#define XNG_ALIGNED_FREE(Ptr) free(Ptr)
#endif

namespace xng
{
    namespace core
    {
        template <typename T, size_t Alignment = std::alignment_of<T>::value>
        struct aligned_allocator :
            allocator_base<T>
        {

            template <class U> struct rebind { typedef aligned_allocator<U, Alignment> other; };

            aligned_allocator(void) = default;
            aligned_allocator(const aligned_allocator &) = default;
            aligned_allocator(aligned_allocator &&) = default;

            template <typename U>
            aligned_allocator(const aligned_allocator<U> &) {}

            inline void * allocate_generic(size_type n, const void * hint = 0)
            {
                void * p = allocate_no_throw_generic(n, hint);

                if (!p)
                {
                    throw std::bad_alloc();
                }

                return p;
            }

            inline pointer allocate(size_type n, const void * hint = 0)
            {
                return static_cast<pointer>(allocate_generic(n, hint));
            }

            inline void * allocate_no_throw_generic(size_type n, const void * hint = 0)
            {
                void * ptr = nullptr;
                XNG_ALIGNED_ALLOC(ptr, n * sizeof(T), Alignment);
                return ptr;
            }

            inline pointer allocate_no_throw(size_type n, const void * hint = 0)
            {
                return static_cast<pointer>(allocate_no_throw_generic(n, hint));
            }

            inline void deallocate(pointer p, size_type n = 1)
            {
                deallocate_generic(static_cast<pointer>(p), n);
            }

            inline void deallocate_generic(void * p, size_type n)
            {
                XNG_ALIGNED_FREE(p);
            }

            template<class U>
            inline aligned_allocator<T, Alignment> & operator= (const aligned_allocator<U, Alignment> &)
            {
                return (*this);
            }

            template <typename U, size_t OtherAlignment>
            inline bool operator== (const aligned_allocator<U, OtherAlignment> &)
            {
                return OtherAlignment == Alignment;
            }

            template <typename U, size_t OtherAlignment>
            inline bool operator!= (const aligned_allocator<U, OtherAlignment> &)
            {
                return OtherAlignment != Alignment;
            }

        };
    }
}

#define XNG_ALIGNED_ALLOCATOR(Type, Alignment)         xng::core::aligned_allocator<Type, Alignment>
#define XNG_DECLARE_ALIGNED_ALLOCATOR(Alignment)       XNG_DECLARE_ALLOCATOR(XNG_ALIGNED_ALLOCATOR(uint8_t, Alignment))
#define XNG_DEFINE_ALIGNED_ALLOCATOR(Class, Alignment) XNG_DEFINE_ALLOCATOR(Class, XNG_ALIGNED_ALLOCATOR(uint8_t, Alignment))