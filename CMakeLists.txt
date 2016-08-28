cmake_minimum_required(VERSION 3.4)

project(xng)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# Compiler settings

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DXNG_DEBUG")

add_definitions(-DXNG_INLINE=inline)

if (WIN32)
	add_definitions(-DNOMINMAX)
	add_definitions(-DXNG_VECTORCALL=__vectorcall)
endif ()

if (MSVC)
	# Need to compile wxWidgets with fast fp to use the following
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /fp:fast")
endif()

# Search Boost includes

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

# Search for OpenGL

list(APPEND CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
list(APPEND CMAKE_LIBRARY_PATH thirdparty/glew/lib)

option(XNG_USE_GLES_2 "Compile with GL ES 2.0 rendering."  OFF)
option(XNG_USE_GLES_3 "Compile with GL ES 3.0 rendering."  OFF)
option(XNG_USE_GL_4_5 "Compile with OpenGL 4.5 rendering." ON)

option(XNG_GL_DEBUG   "Add checks and overhead to help debugging OpenGL code."    ON)
option(XNG_DX11_DEBUG "Add checks and overhead to help debugging DirectX11 code." ON)

# Gather XNG components to compile

set(XNG_COMPONENTS core res math graphics os editor launcher)

if (XNG_USE_GLES_2_0 OR XNG_USE_GLES_3_0 OR XNG_USE_GL_4_5)

	set(XNG_USE_GL ON)
	set(XNG_GL_SHADERS_DIR ${CMAKE_SOURCE_DIR}/gl/shaders)
	
	add_definitions(-DXNG_GL)
	
	if (XNG_GL_DEBUG)
		add_definitions(-DXNG_GL_DEBUG)
	endif ()
	
	if (XNG_USE_GL_ES_2_0)
		add_definitions(-DXNG_GLES_2)
	endif ()
	
	if (XNG_USE_GL_ES_3_0)
		add_definitions(-DXNG_GLES_3)
	endif ()
	
	if (XNG_USE_GL_4_5)
		add_definitions(-DXNG_GL_4_5)
	endif ()
	
	find_package(OpenGL REQUIRED)
	find_package(GLEW)
	
	if (GLEW_FOUND OR OPENGL_FOUND)
		set(XNG_GL_LIBRARIES ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
		set(XNG_GL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
	endif ()
	
	include_directories(${XNG_GL_INCLUDE_DIRS})
	
	set(XNG_COMPONENTS ${XNG_COMPONENTS} gl)
	
endif ()


if (WIN32)

	add_definitions(-DXNG_WIN32)	
	set(XNG_COMPONENTS ${XNG_COMPONENTS} win32)
	add_definitions(-DXNG_GL_SHADER_FOLDER="./shaders/gl/")
	
	option(XNG_USE_DX11 "Compile with DirectX 11 rendering." ON)
	
	if (XNG_USE_DX11)
	
		if (XNG_DX11_DEBUG)
			add_definitions(-DXNG_DX11_DEBUG)
			add_definitions("-DXNG_DX11_GLOBAL_SHADER_FLAGS=D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_OPTIMIZATION_LEVEL0")
		endif ()
	
		set(XNG_DX11_SHADERS_DIR ${CMAKE_SOURCE_DIR}/dx11/shaders)
		add_definitions(-DXNG_DX11_SHADER_FOLDER="./shaders/dx11/")
		
		find_package(DirectX11 REQUIRED)
		
		add_definitions(-DXNG_DX11)
		
		set(XNG_COMPONENTS ${XNG_COMPONENTS} dx11)
		set(XNG_DX11_LIBRARIES ${DirectX11_LIBRARY})
		
	endif()
	
endif()

# Utility function to add a component

function(xng_components_add)
	foreach(component IN LISTS ARGV)
		include_directories(${component}/include)
	endforeach()
	
	foreach(component IN LISTS ARGV)
		add_subdirectory(${component})
	endforeach()
endfunction(xng_components_add)

# Add the components

xng_components_add(${XNG_COMPONENTS})

# Test stuff

add_subdirectory(resource_test)
add_subdirectory(math_test)