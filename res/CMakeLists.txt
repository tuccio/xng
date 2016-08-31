cmake_minimum_required(VERSION 3.4)

project(xngres)

file(GLOB XNG_RES_SOURCE_FILES *.cpp)
file(GLOB XNG_RES_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/res/*.hpp include/xng/res/*.inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngresexp STATIC ${XNG_RES_SOURCE_FILES} ${XNG_RES_INCLUDE_FILES})
add_library(xngres STATIC ${XNG_RES_SOURCE_FILES} ${XNG_RES_INCLUDE_FILES})