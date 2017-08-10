cmake_minimum_required(VERSION 3.4)

option(XNG_USE_VULKAN "Compile with Vulkan rendering." ON)
option(XNG_VK_DEBUG "Add checks and overhead to help debugging Vulkan code." ON)

if (XNG_USE_VULKAN)

	project(xngvk)

	if (XNG_VK_DEBUG)
		add_definitions(-DXNG_VK_DEBUG)
	endif ()
	
	set(XNG_VK_SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
	add_definitions(-DXNG_DX11_SHADER_FOLDER="./shaders/dx11/")
	
	find_package(Vulkan REQUIRED)
	
	add_definitions(-DXNG_VK)

	file(GLOB XNG_VK_CORE_SOURCE_FILES src/core/*.cpp)
	file(GLOB XNG_VK_CORE_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/vk/core/*.hpp include/xng/vk/core/*.inl)
    
    file(GLOB XNG_VK_MODULES_SOURCE_FILES src/modules/*.cpp)
	file(GLOB XNG_VK_MODULES_INCLUDE_FILES include/xng/vk/modules/*.hpp include/xng/vk/modules/*.inl)
	
	set(XNG_VK_SOURCE_FILES ${XNG_VK_CORE_SOURCE_FILES} ${XNG_VK_MODULES_SOURCE_FILES})
	set(XNG_VK_INCLUDE_FILES ${XNG_VK_CORE_INCLUDE_FILES} ${XNG_VK_MODULES_INCLUDE_FILES})
    
    source_group("core\\Header Files" FILES ${XNG_VK_CORE_INCLUDE_FILES})
	source_group("core\\Source Files" FILES ${XNG_VK_CORE_SOURCE_FILES})
    
    source_group("modules\\Header Files" FILES ${XNG_VK_MODULES_INCLUDE_FILES})
	source_group("modules\\Source Files" FILES ${XNG_VK_MODULES_SOURCE_FILES})
	
	include_directories(${XNG_INCLUDE_DIRS})
	include_directories(include)
	
	add_library(xngvk SHARED ${XNG_VK_SOURCE_FILES} ${XNG_VK_INCLUDE_FILES})
	
	target_link_libraries(xngvk ${XNG_LIBRARIES} Vulkan::Vulkan)
	xng_link_module(xngvk)
	
	add_custom_command(TARGET xngvk PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${XNG_VK_SHADERS_DIR} $<TARGET_FILE_DIR:xngvk>/shaders)
	
endif ()