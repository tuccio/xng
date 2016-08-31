cmake_minimum_required(VERSION 3.4)

project(xnggraphics)

file(GLOB XNG_GRAPHICS_SOURCE_FILES *.cpp)
file(GLOB XNG_GRAPHICS_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/graphics/*.hpp include/xng/graphics/*.inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xnggraphicsexp STATIC ${XNG_GRAPHICS_SOURCE_FILES} ${XNG_GRAPHICS_INCLUDE_FILES})
add_library(xnggraphics STATIC ${XNG_GRAPHICS_SOURCE_FILES} ${XNG_GRAPHICS_INCLUDE_FILES})