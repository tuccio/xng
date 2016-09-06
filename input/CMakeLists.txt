cmake_minimum_required(VERSION 3.4)

project(xnginput)

file(GLOB XNG_INPUT_SOURCE_FILES *.cpp)
file(GLOB XNG_INPUT_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/input/*.hpp include/xng/input/*.inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xnginputexp STATIC ${XNG_INPUT_SOURCE_FILES} ${XNG_INPUT_INCLUDE_FILES})
add_library(xnginput STATIC ${XNG_INPUT_SOURCE_FILES} ${XNG_INPUT_INCLUDE_FILES})