cmake_minimum_required(VERSION 3.4)

project(xnggui)

file(GLOB XNG_GUI_SOURCE_FILES *.cpp)
file(GLOB XNG_GUI_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/gui/*.hpp include/xng/gui/*.inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngguiexp STATIC ${XNG_GUI_SOURCE_FILES} ${XNG_GUI_INCLUDE_FILES})
add_library(xnggui STATIC ${XNG_GUI_SOURCE_FILES} ${XNG_GUI_INCLUDE_FILES})