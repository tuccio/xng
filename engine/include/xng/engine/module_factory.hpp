#pragma once

#include <xng/engine/module.hpp>

enum xng_module_type
{
	XNG_MODULE_TYPE_INPUT,
	XNG_MODULE_TYPE_RUNTIME,
	XNG_MODULE_TYPE_SCRIPT,
	XNG_MODULE_TYPE_PHYSICS,
	XNG_MODULE_TYPE_SCENE,
	XNG_MODULE_TYPE_ANIMATION,
	XNG_MODULE_TYPE_AUDIO,
	XNG_MODULE_TYPE_RENDER
};

namespace xng
{
	namespace engine
	{
		struct module_factory
		{
			virtual ~module_factory(void);

			virtual module * create(void) const = 0;
			virtual void destroy(module * module) const = 0;

			virtual const char * name(void) const = 0;
			virtual const char * description(void) const = 0;
			virtual xng_module_type type(void) const = 0;			
		};

		template <typename Module>
		struct basic_module_factory :
			public module_factory
		{
			module * create(void) const override
			{
				return xng_new Module;
			}

			void destroy(module * module) const override
			{
				xng_delete module;
			}

			const char * name(void) const override
			{
				return Module::module_name;
			}

			const char * description(void) const override
			{
				return Module::module_description;
			}

			xng_module_type type(void) const override
			{
				return Module::module_type;
			}
		};

	}
}

#include <boost/preprocessor.hpp>
#include <boost/preprocessor/seq.hpp>

#define XNG_MODULE_STRINGIZE_2(Value) #Value
#define XNG_MODULE_STRINGIZE(Value)   XNG_MODULE_STRINGIZE_2(Value)

#define XNG_EXPORT_MODULES_FSYM  xng_export_modules
#define XNG_EXPORT_MODULES_FNAME XNG_MODULE_STRINGIZE(XNG_EXPORT_MODULES_FSYM)

#define XNG_CLEANUP_MODULES_FSYM  xng_cleanup_modules
#define XNG_CLEANUP_MODULES_FNAME XNG_MODULE_STRINGIZE(XNG_CLEANUP_MODULES_FSYM)

#define XNG_MODULE_FACTORY_ARRAY_FSYM s_xngExportedModules

#define XNG_MODULE_FACTORY_ARRAY_INSTANTIATE(r, data, Module) xng_new xng::engine::basic_module_factory<Module>() BOOST_PP_COMMA()
#define XNG_MODULE_FACTORY_ARRAY_FILL(...) BOOST_PP_SEQ_FOR_EACH(XNG_MODULE_FACTORY_ARRAY_INSTANTIATE, 0, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) nullptr

#define XNG_CLEANUP_MODULES() extern "C" XNG_EXPORT void XNG_CLEANUP_MODULES_FSYM (void) { xng::engine::module_factory ** modules = XNG_EXPORT_MODULES_FSYM(); for (xng::engine::module_factory ** module = modules; *module != nullptr; ++module) xng_delete module; }
#define XNG_MODULE_FACTORY_ARRAY(...) static xng::engine::module_factory * XNG_MODULE_FACTORY_ARRAY_FSYM [] = { XNG_MODULE_FACTORY_ARRAY_FILL(__VA_ARGS__) };
#define XNG_EXPORT_MODULES(...) extern "C" XNG_EXPORT xng::engine::module_factory ** XNG_EXPORT_MODULES_FSYM (void) { XNG_MODULE_FACTORY_ARRAY(__VA_ARGS__); return XNG_MODULE_FACTORY_ARRAY_FSYM; } XNG_CLEANUP_MODULES()