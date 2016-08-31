cmake_minimum_required(VERSION 3.4)

option(XNG_USE_GLES_2 "Compile with GL ES 2.0 rendering."  OFF)
option(XNG_USE_GLES_3 "Compile with GL ES 3.0 rendering."  OFF)
option(XNG_USE_GL_4_5 "Compile with OpenGL 4.5 rendering." ON)

option(XNG_GL_DEBUG   "Add checks and overhead to help debugging OpenGL code." ON)

if (XNG_USE_GLES_2_0 OR XNG_USE_GLES_3_0 OR XNG_USE_GL_4_5)
	
		project(xnggl)

		set(XNG_USE_GL ON PARENT_SCOPE)
		set(XNG_GL_SHADERS_DIR ${CMAKE_SOURCE_DIR}/gl/shaders)
		add_definitions(-DXNG_GL_SHADER_FOLDER="./shaders/gl/")
		
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
		
		
		file(GLOB XNG_GL_SOURCE_FILES *.cpp)
		file(GLOB XNG_GL_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/gl/*.hpp include/xng/gl/*.inl)		
		
		include_directories(include ${XNG_INCLUDE_DIRS} ${XNG_GL_INCLUDE_DIRS})
		add_library(xnggl SHARED ${XNG_GL_SOURCE_FILES} ${XNG_GL_INCLUDE_FILES})
		
		target_link_libraries(xnggl ${XNG_LIBRARIES} ${XNG_GL_LIBRARIES})
		
		add_custom_command(TARGET xnggl PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${XNG_GL_SHADERS_DIR} $<TARGET_FILE_DIR:xnggl>/shaders)
	
	endif ()