cmake_minimum_required(VERSION 3.4)

project(xng)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Compiler settings

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DXNG_DEBUG")

add_definitions(-DXNG_INLINE=inline)

if (WIN32)
	add_definitions(-DNOMINMAX)
endif ()

if (MSVC)
	# Need to compile wxWidgets with fast fp to use the following
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /fp:fast")
	add_definitions("-DXNG_EXPORT=__declspec(dllexport)")
	add_definitions("-DXNG_IMPORT=__declspec(dllimport)")
endif()

# Search Boost includes

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

# Search for OpenGL

list(APPEND CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
list(APPEND CMAKE_LIBRARY_PATH thirdparty/glew/lib)

# Gather XNG components to compile

function(xng_add_components)
	foreach(component IN LISTS ARGV)
		set(XNG_INCLUDE_DIRS ${XNG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${component}/include)
		set(XNG_IMPORT_LIBRARIES ${XNG_IMPORT_LIBRARIES} xng${component})
		set(XNG_EXPORT_LIBRARIES ${XNG_EXPORT_LIBRARIES} xng${component}exp)
		set(XNG_COMPONENTS ${XNG_COMPONENTS} ${component})
	endforeach()
	set(XNG_INCLUDE_DIRS ${XNG_INCLUDE_DIRS} PARENT_SCOPE)
	set(XNG_IMPORT_LIBRARIES ${XNG_IMPORT_LIBRARIES} PARENT_SCOPE)
	set(XNG_EXPORT_LIBRARIES ${XNG_EXPORT_LIBRARIES} PARENT_SCOPE)
	set(XNG_COMPONENTS ${XNG_COMPONENTS} PARENT_SCOPE)
endfunction(xng_add_components)

function(xng_make_components_projects)
	foreach(component IN LISTS XNG_COMPONENTS)
		add_subdirectory(${component})
	endforeach()
endfunction(xng_make_components_projects)

xng_add_components(core res math graphics engine)

if (WIN32)
	add_definitions(-DXNG_WIN32)
	xng_add_components(win32)
endif()

xng_make_components_projects()

# Exporter

add_subdirectory(exporter)
set(XNG_LIBRARIES ${XNG_IMPORT_LIBRARIES} xngexporter)

# Modules

add_subdirectory(gl)
add_subdirectory(dx11)
add_subdirectory(editor)

# Launcher

add_subdirectory(launcher)

# Export linking

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

xng_make_exporter(${XNG_EXPORT_LIBRARIES} xngexporter)
xng_make_importer(${XNG_IMPORT_LIBRARIES} xnglauncher xnggl xngdx11 xngeditor)

# Test stuff

add_subdirectory(resource_test)
add_subdirectory(math_test)