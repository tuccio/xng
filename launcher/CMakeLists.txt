cmake_minimum_required(VERSION 3.4)

project(xnglauncher)

find_package(Boost COMPONENTS program_options REQUIRED)

file(GLOB XNG_LAUNCHER_SOURCE_FILES *.cpp)
file(GLOB XNG_LAUNCHER_INCLUDE_FILES *.hpp *inl)

if (WIN32)
	set(XNG_LAUNCHER_PLATFORM WIN32)
endif ()

include_directories(${XNG_INCLUDE_DIRS})

add_executable(xnglauncher ${XNG_LAUNCHER_PLATFORM} ${XNG_LAUNCHER_SOURCE_FILES} ${XNG_LAUNCHER_INCLUDE_FILES})
target_link_libraries(xnglauncher ${XNG_LIBRARIES})

xng_set_modules_dependency(xnglauncher)
xng_link_dependencies(xnglauncher)