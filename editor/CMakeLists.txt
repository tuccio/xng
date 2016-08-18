cmake_minimum_required(VERSION 3.4)

project(xngeditor)

find_package(wxWidgets COMPONENTS core base aui REQUIRED)

add_definitions(-DUNICODE -D_UNICODE)
include(${wxWidgets_USE_FILE})

file(GLOB XNG_EDITOR_SOURCE_FILES *.cpp)
file(GLOB XNG_EDITOR_INCLUDE_FILES *.hpp *inl)

if (WIN32)
	set(XNG_EDITOR_PLATFORM WIN32)
endif ()

add_executable(xngeditor ${XNG_EDITOR_PLATFORM} ${XNG_EDITOR_SOURCE_FILES} ${XNG_EDITOR_INCLUDE_FILES})

if (WIN32)
	target_link_libraries(xngeditor xngwin32 ${wxWidgets_LIBRARIES})
endif ()

target_link_libraries(xngeditor xngcore xngres xnggraphics)

if (XNG_USE_GL)
	target_link_libraries(xngeditor xnggl ${XNG_GL_LIBRARIES})
	add_custom_command(TARGET xngeditor PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${XNG_GL_SHADERS_DIR} $<TARGET_FILE_DIR:xngeditor>/shaders )
endif ()

if (XNG_USE_DX11)
	target_link_libraries(xngeditor xngdx11 ${XNG_DX11_LIBRARIES})
	add_custom_command(TARGET xngeditor PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${XNG_DX11_SHADERS_DIR} $<TARGET_FILE_DIR:xngeditor>/shaders )
endif ()