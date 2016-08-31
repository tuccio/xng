cmake_minimum_required(VERSION 3.4)

project(xngengine)

file(GLOB XNG_ENGINE_SOURCE_FILES *.cpp)
file(GLOB XNG_ENGINE_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/engine/*.hpp include/xng/engine/*.inl)
	
include_directories(${XNG_INCLUDE_DIRS})

add_library(xngengineexp STATIC ${XNG_ENGINE_SOURCE_FILES} ${XNG_ENGINE_INCLUDE_FILES})
add_library(xngengine STATIC ${XNG_ENGINE_SOURCE_FILES} ${XNG_ENGINE_INCLUDE_FILES})