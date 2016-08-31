cmake_minimum_required(VERSION 3.4)

if (WIN32)

	project(xngwin32)
	
	file(GLOB XNG_WIN32_SOURCE_FILES *.cpp)
	file(GLOB XNG_WIN32_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/os/*.hpp include/xng/os/*.inl)
	
	include_directories(${XNG_INCLUDE_DIRS})
	add_library(xngwin32exp STATIC ${XNG_WIN32_SOURCE_FILES} ${XNG_WIN32_INCLUDE_FILES})
	
	add_definitions(${XNG_SHARE_IMPORTER})
	add_library(xngwin32 STATIC ${XNG_WIN32_SOURCE_FILES} ${XNG_WIN32_INCLUDE_FILES})
	
endif ()