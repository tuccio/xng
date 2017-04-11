#include <xng/core/refcounted_ptr.hpp>
#include <xng/core/debug_new.hpp>

#include <atomic>

namespace xng
{
    namespace core
    {
        template <typename T>
        class refcounted_obj
        {

        protected:

            refcounted_obj(void) : m_references(0) {}

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
                    xng_delete (T*)this;
                }
            }

            template <typename U>
            friend class refcounted_ptr;
        };
    }
}