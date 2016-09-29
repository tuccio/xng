cmake_minimum_required(VERSION 3.4)

project(xng)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

include(xng)

# Compiler settings

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DXNG_DEBUG")

option(XNG_DEBUG_NEW    "Use memory allocation logging to debug memory leaks" OFF)
option(XNG_USE_FREETYPE "Compile with freetype font loader" OFF)
option(XNG_USE_ASSIMP   "Compile with Assimp support for loading scenes" OFF)

if (XNG_DEBUG_NEW)
		add_definitions(-DXNG_DEBUG_NEW)
endif()

add_definitions(-DXNG_INLINE=inline)

if (WIN32)
	add_definitions(-DNOMINMAX)
endif ()

if (MSVC)
	# Need to compile all libraries with fast fp to use the following
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /fp:fast")
	add_definitions(-DXNG_MSVC)
endif()

# Search Boost includes

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

# Search Assimp if used

if (XNG_USE_ASSIMP)

	find_package(Assimp REQUIRED)
	
	add_definitions(-DXNG_ASSIMP)
	include_directories(${ASSIMP_INCLUDE_DIR})
	xng_add_dependencies(${ASSIMP_LIBRARY})
	
endif()

# Search freetype2 if used

if (XNG_USE_FREETYPE)

	find_package(freetype REQUIRED)
	find_package(bzip2 REQUIRED)
	
	include_directories(${FREETYPE_INCLUDE_DIRS})
	xng_add_dependencies(${FREETYPE_LIBRARIES} ${BZIP2_LIBRARIES})
	add_definitions(-DXNG_FREETYPE)
	
endif()

# Search OpenMP

find_package(OpenMP)

if (OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Search libpng

set(PNG_DEFINITIONS -DPNG_STATIC)
find_package(PNG REQUIRED)

# Add glew path for find_package

list(APPEND CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
list(APPEND CMAKE_LIBRARY_PATH thirdparty/glew/lib)

# Add TCLAP to the include_directories

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/tclap/include)

# Gather XNG components to compile

xng_add_components(core res math graphics input engine gui)

if (WIN32)
	add_definitions(-DXNG_WIN32)
	xng_add_os_library(win32)
endif()

xng_make_components_projects()

# Exporter

add_subdirectory(exporter)
set(XNG_LIBRARIES ${XNG_IMPORT_LIBRARIES} xngexporter)

# Modules

xng_add_modules(scene gl dx11 editor)
xng_make_modules_projects()

# Launcher

add_subdirectory(launcher)

# Font baker

if (XNG_USE_FREETYPE)
	add_subdirectory(fontbaker)
endif ()

# Export linking

xng_make_exporter(${XNG_EXPORT_LIBRARIES} xngexporter)
xng_make_importer(${XNG_IMPORT_LIBRARIES} ${XNG_MODULES_LIBRARIES} xnglauncher xngfontbaker)

# Test stuff

add_subdirectory(resource_test)
add_subdirectory(math_test)

xng_make_importer(resource_test math_test)