cmake_minimum_required(VERSION 3.4)

project(xnggraphics)

file(GLOB XNG_GRAPHICS_SOURCE_FILES *.cpp)
file(GLOB XNG_GRAPHICS_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/graphics/*.hpp include/xng/graphics/*.inl)

add_library(xnggraphics STATIC ${XNG_GRAPHICS_SOURCE_FILES} ${XNG_GRAPHICS_INCLUDE_FILES})