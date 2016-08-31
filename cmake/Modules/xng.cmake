# XNG Library Setup

unset(XNG_INCLUDE_DIRS CACHE)
unset(XNG_IMPORT_LIBRARIES CACHE)
unset(XNG_EXPORT_LIBRARIES CACHE)
unset(XNG_COMPONENTS CACHE)

function(xng_add_components)
	foreach(component IN LISTS ARGV)
		set(XNG_INCLUDE_DIRS ${XNG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${component}/include CACHE INTERNAL "XNG Library include directories.")
		set(XNG_IMPORT_LIBRARIES ${XNG_IMPORT_LIBRARIES} xng${component} CACHE INTERNAL "XNG Library components compiled to import shared symbols.")
		set(XNG_EXPORT_LIBRARIES ${XNG_EXPORT_LIBRARIES} xng${component}exp CACHE INTERNAL "XNG Library components compiled to export shared symbols.")
		set(XNG_COMPONENTS ${XNG_COMPONENTS} ${component} CACHE INTERNAL "XNG Library components.")
	endforeach()
endfunction(xng_add_components)

function(xng_make_components_projects)
	foreach(component IN LISTS XNG_COMPONENTS)
		add_subdirectory(${component})
	endforeach()
endfunction(xng_make_components_projects)

# XNG Modules Setup

unset(XNG_MODULES_LIBRARIES CACHE)
unset(XNG_MODULES CACHE)

function(xng_add_modules)
	foreach(module IN LISTS ARGV)
		set(XNG_MODULES ${XNG_MODULES} ${module} CACHE INTERNAL "XNG Modules compiled.")
	endforeach()
endfunction(xng_add_modules)

function(xng_make_modules_projects)
	foreach(module IN LISTS XNG_MODULES)
		add_subdirectory(${module})
	endforeach()
endfunction(xng_make_modules_projects)

function(xng_link_module module)
	set(XNG_MODULES_LIBRARIES ${XNG_MODULES_LIBRARIES} ${module} CACHE INTERNAL "XNG Modules compiled.")
endfunction(xng_link_module)

function(xng_set_modules_dependency target)
	add_dependencies(${target} ${XNG_MODULES_LIBRARIES})
endfunction(xng_set_modules_dependency)

# XNG Export macros

function(xng_make_exporter)
	foreach(target IN LISTS ARGV)
		target_compile_definitions(${target} PRIVATE -DXNG_SHARE_EXPORTER -DXNG_SHARE=XNG_EXPORT)
	endforeach()
endfunction(xng_make_exporter)

function(xng_make_importer)
	foreach(target IN LISTS ARGV)
		target_compile_definitions(${target} PRIVATE -DXNG_SHARE_IMPORTER -DXNG_SHARE=XNG_IMPORT)
	endforeach()
endfunction(xng_make_importer)