cmake_minimum_required(VERSION 3.4)

project(xnggl)

if (XNG_USE_GL)

	file(GLOB XNG_GL_SOURCE_FILES *.cpp)
	file(GLOB XNG_GL_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/gl/*.hpp include/xng/gl/*.inl)
	
	add_library(xnggl STATIC ${XNG_GL_SOURCE_FILES} ${XNG_GL_INCLUDE_FILES})
	
endif ()