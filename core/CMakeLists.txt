cmake_minimum_required(VERSION 3.4)

project(xngcore)

file(GLOB XNG_CORE_SOURCE_FILES *.cpp)
file(GLOB XNG_CORE_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/core/*.hpp include/xng/core/*.inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngcoreexp STATIC ${XNG_CORE_SOURCE_FILES} ${XNG_CORE_INCLUDE_FILES})
add_library(xngcore STATIC ${XNG_CORE_SOURCE_FILES} ${XNG_CORE_INCLUDE_FILES})