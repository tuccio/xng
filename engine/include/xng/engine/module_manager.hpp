#pragma once

#include <xng/core.hpp>
#include <xng/os.hpp>
#include <xng/engine/module_factory.hpp>

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace xng
{
    namespace engine
    {

        typedef module_factory ** (*export_modules_t) (void);
        typedef void (*cleanup_modules_t) (void);

        class module_manager :
            public core::singleton<module_manager>
        {
            
        public:

            ~module_manager(void);

            void clear(void);

            std::vector<module_factory*> get_modules(void) const;

            module_factory * find_module_by_name(const char * name);
            module_factory * find_module_by_type(xng_module_type type);

            bool register_shared_library(const os::path & path);
            void unregister_shared_library(const os::path & path);

        private:

            std::vector<module_factory*> m_modules;
            std::unordered_map<module_factory*, os::shared_library*> m_libraries;

            mutable std::mutex m_mutex;

            void register_module(module_factory * factory);
            void unregister_module(module_factory * factory);

        };
    }
}