cmake_minimum_required(VERSION 2.8)

project(xngmath)

file(GLOB XNG_MATH_SOURCE_FILES math/*.cpp)
file(GLOB XNG_MATH_INCLUDE_FILES include/xng/math/*.hpp include/xng/math/*.inl include/xng/math.hpp)

file(GLOB XNG_GEOMETRY_SOURCE_FILES geometry/*.cpp)
file(GLOB XNG_GEOMETRY_INCLUDE_FILES include/xng/geometry/*.hpp include/xng/geometry/*.inl include/xng/geometry.hpp)

source_group("math\\include" FILES ${XNG_MATH_INCLUDE_FILES})
source_group("math\\source" FILES ${XNG_MATH_SOURCE_FILES})

source_group("geometry\\include" FILES ${XNG_GEOMETRY_INCLUDE_FILES})
source_group("geometry\\source" FILES ${XNG_GEOMETRY_SOURCE_FILES})

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngmath ${XNG_MATH_INCLUDE_FILES} ${XNG_MATH_SOURCE_FILES} ${XNG_GEOMETRY_INCLUDE_FILES} ${XNG_GEOMETRY_SOURCE_FILES})
add_library(xngmathexp ${XNG_MATH_INCLUDE_FILES} ${XNG_MATH_SOURCE_FILES} ${XNG_GEOMETRY_INCLUDE_FILES} ${XNG_GEOMETRY_SOURCE_FILES})