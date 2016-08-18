cmake_minimum_required(VERSION 3.4)

if (WIN32)

	project(xngwin32)
	
	file(GLOB XNG_WIN32_SOURCE_FILES *.cpp)
	file(GLOB XNG_WIN32_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/win32/*.hpp include/xng/win32/*.inl)
	
	add_library(xngwin32 STATIC ${XNG_WIN32_SOURCE_FILES} ${XNG_WIN32_INCLUDE_FILES})
	
endif ()