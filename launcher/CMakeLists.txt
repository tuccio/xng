cmake_minimum_required(VERSION 3.4)

project(xnglauncher)

file(GLOB XNG_LAUNCHER_SOURCE_FILES *.cpp)
file(GLOB XNG_LAUNCHER_INCLUDE_FILES *.hpp *inl)

if (WIN32)
	set(XNG_LAUNCHER_PLATFORM WIN32)
endif ()

include_directories(${XNG_INCLUDE_DIRS})

add_executable(xnglauncher ${XNG_LAUNCHER_PLATFORM} ${XNG_LAUNCHER_SOURCE_FILES} ${XNG_LAUNCHER_INCLUDE_FILES})
target_link_libraries(xnglauncher ${XNG_LIBRARIES})

if (XNG_USE_GL)
	add_dependencies(xnglauncher xnggl)
endif ()

if (XNG_USE_DX11)
	add_dependencies(xnglauncher xngdx11)
endif ()

add_dependencies(xnglauncher xngeditor)