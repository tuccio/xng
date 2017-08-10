cmake_minimum_required(VERSION 3.4)

if (WIN32)
	option(XNG_USE_DX11 "Compile with DirectX 11 rendering." ON)
	option(XNG_DX11_DEBUG "Add checks and overhead to help debugging DirectX11 code." ON)
endif ()

if (XNG_USE_DX11)

	project(xngdx11)

	if (XNG_DX11_DEBUG)
		add_definitions(-DXNG_DX11_DEBUG)
		add_definitions("-DXNG_DX11_GLOBAL_SHADER_FLAGS=D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_OPTIMIZATION_LEVEL0")
	endif ()
	
	set(XNG_DX11_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
	add_definitions(-DXNG_DX11_SHADER_FOLDER="./shaders/dx11/")
	
	find_package(DirectX11 REQUIRED)
	
	add_definitions(-DXNG_DX11)

	file(GLOB XNG_DX11_CORE_SOURCE_FILES *.cpp)
	file(GLOB XNG_DX11_CORE_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/dx11/*.hpp include/xng/dx11/*.inl)
	
	file(GLOB XNG_DX11_SHADOWS_SOURCE_FILES shadows/*.cpp)
	file(GLOB XNG_DX11_SHADOWS_INCLUDE_FILES include/xng/dx11/shadows/*.hpp include/xng/dx11/shadows/*.inl)
	
	set(XNG_DX11_SOURCE_FILES ${XNG_DX11_CORE_SOURCE_FILES} ${XNG_DX11_SHADOWS_SOURCE_FILES})
	set(XNG_DX11_INCLUDE_FILES ${XNG_DX11_CORE_INCLUDE_FILES} ${XNG_DX11_SHADOWS_INCLUDE_FILES})
	
	source_group("shadows\\Header Files" FILES ${XNG_DX11_SHADOWS_INCLUDE_FILES})
	source_group("shadows\\Source Files" FILES ${XNG_DX11_SHADOWS_SOURCE_FILES})
	
	include_directories(${XNG_INCLUDE_DIRS})
	include_directories(include)
	
	add_library(xngdx11 SHARED ${XNG_DX11_SOURCE_FILES} ${XNG_DX11_INCLUDE_FILES})
	
	target_link_libraries(xngdx11 ${XNG_LIBRARIES} ${DirectX11_LIBRARY})
	xng_link_module(xngdx11)
	
	add_custom_command(TARGET xngdx11 PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${XNG_DX11_SHADERS_DIR} $<TARGET_FILE_DIR:xngdx11>/shaders)
	
endif ()