#pragma once

#include <cstdint>
#include <memory>
#include <malloc.h>

namespace xng
{
    namespace core
    {
        template <typename T>
        struct allocator_base
        {

            typedef T value_type;
            typedef T * pointer;
            typedef const T * const_pointer;
            typedef T & reference;
            typedef const T & const_reference;
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;

            template <typename ... Args>
            void construct(T * p, Args && ... args)
            {
                XNG_NEW (p) T(args ...);
            }

            void destroy(T * p)
            {
                p->~T();
            }

        };
    }
}

#define XNG_ALLOCATOR_STATIC_MEMBER s_xngAllocator

#define XNG_DECLARE_ALLOCATOR(Allocator) \
private:\
    static Allocator XNG_ALLOCATOR_STATIC_MEMBER;\
public:\
    inline void * operator new(size_t size) { return XNG_ALLOCATOR_STATIC_MEMBER.allocate_generic(size); }\
    inline void * operator new(size_t size, const std::nothrow_t &) { return XNG_ALLOCATOR_STATIC_MEMBER.allocate_no_throw_generic(size); }\
    inline void * operator new(size_t size, void * p){ return p; }\
    inline void operator delete(void * p) { XNG_ALLOCATOR_STATIC_MEMBER.deallocate_generic(p, 1); }\
    inline void operator delete(void * p, void * p2) { }

#define XNG_DEFINE_ALLOCATOR(Class, Allocator) Allocator Class::XNG_ALLOCATOR_STATIC_MEMBER;