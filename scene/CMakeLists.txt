cmake_minimum_required(VERSION 3.4)

project(xngscene)

file(GLOB XNG_SCENE_SOURCE_FILES *.cpp)
file(GLOB XNG_SCENE_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/scene/*.hpp include/xng/scene/*.inl)

include_directories(${XNG_INCLUDE_DIRS} include)

add_library(xngscene SHARED ${XNG_SCENE_SOURCE_FILES} ${XNG_SCENE_INCLUDE_FILES})
xng_link_module(xngscene)

target_link_libraries(xngscene ${XNG_LIBRARIES})