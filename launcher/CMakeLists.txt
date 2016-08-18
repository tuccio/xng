cmake_minimum_required(VERSION 3.4)

project(xnglauncher)

file(GLOB XNG_LAUNCHER_SOURCE_FILES *.cpp)
file(GLOB XNG_LAUNCHER_INCLUDE_FILES *.hpp *inl)

if (WIN32)
	set(XNG_LAUNCHER_PLATFORM WIN32)
endif ()

add_executable(xnglauncher ${XNG_LAUNCHER_PLATFORM} ${XNG_LAUNCHER_SOURCE_FILES} ${XNG_LAUNCHER_INCLUDE_FILES})

if (WIN32)
	target_link_libraries(xnglauncher xngwin32)
endif ()

target_link_libraries(xnglauncher xngcore xnggraphics)
target_link_libraries(xnglauncher xnggl ${XNG_GL_LIBRARIES})