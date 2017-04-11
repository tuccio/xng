#pragma once

#include <functional>
#include <memory>
#include <atomic>

namespace xng
{
    namespace res
    {

        class resource;

        class resource_loader :
            public core::refcounted_obj<resource_loader>
        {

        public:

            virtual ~resource_loader(void) = default;

            virtual bool load(resource *, const void * userdata) = 0;
            virtual void unload(resource *) = 0;

        };

        /*class resource_loader
        {

        public:

            resource_loader(void) :
                m_references(0) {}

            virtual ~resource_loader(void) = default;

            virtual bool load(resource *, const void * userdata) = 0;
            virtual void unload(resource *) = 0;

        private:

            mutable std::atomic<int> m_references;

            void ref(void) const
            {
                m_references.fetch_add(1, std::memory_order_relaxed);
            }

            void unref(void) const
            {
                if (m_references.fetch_sub(1, std::memory_order_release) == 1)
                {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    xng_delete this;
                }
            }

            template <typename Resource>
            friend class xng::core::refcounted_ptr;

        };*/

        typedef xng::core::refcounted_ptr<resource_loader> resource_loader_ptr;

        template <typename Loader, typename ... Args>
        resource_loader_ptr make_resource_loader(Args && ... args)
        {
            return resource_loader_ptr(xng_new Loader(std::forward<Args>(args) ...));
        }

        class dynamic_resource_loader :
            public resource_loader
        {

        public:

            dynamic_resource_loader(void) = default;

            template <typename Loader, typename Unloader>
            dynamic_resource_loader(Loader && loader, Unloader && unloader) :
                m_loader(std::forward<Loader>(loader)),
                m_unloader(std::forward<Unloader>(unloader)) {}

        protected:

            bool load(resource * r, const void * userdata) override { assert(m_loader && m_unloader && "Cannot use dynamic_resource_loader without proper initialization"); return m_loader(r, userdata); }
            void unload(resource * r) override { assert(m_unloader && m_unloader && "Cannot use dynamic_resource_loader without proper initialization"); m_unloader(r); }

        private:

            std::function<bool(resource *, const void *)> m_loader;
            std::function<void(resource *)>               m_unloader;

        };

    }
}