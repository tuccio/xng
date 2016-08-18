cmake_minimum_required(VERSION 3.4)

project(xngos)

file(GLOB XNG_OS_SOURCE_FILES *.cpp)
file(GLOB XNG_OS_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/os/*.hpp include/xng/os/*.inl)

add_library(xngos ${XNG_OS_SOURCE_FILES} ${XNG_OS_INCLUDE_FILES})
set_target_properties(xngos PROPERTIES LINKER_LANGUAGE CXX)