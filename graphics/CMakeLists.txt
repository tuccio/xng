cmake_minimum_required(VERSION 3.4)

project(xnggraphics)

file(GLOB XNG_GRAPHICS_SOURCE_FILES *.cpp)
file(GLOB XNG_GRAPHICS_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/graphics/*.hpp include/xng/graphics/*.inl)

list(REMOVE_ITEM XNG_GRAPHICS_SOURCE_FILES signed_distance_field.cpp)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xnggraphicsexp STATIC ${XNG_GRAPHICS_SOURCE_FILES} ${XNG_GRAPHICS_INCLUDE_FILES})
add_library(xnggraphics STATIC ${XNG_GRAPHICS_SOURCE_FILES} ${XNG_GRAPHICS_INCLUDE_FILES})

include_directories(${PNG_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})

target_link_libraries(xnggraphicsexp ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})
target_link_libraries(xnggraphics ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})

add_library(xngcpugraphics SHARED signed_distance_field.cpp)
add_dependencies(xnggraphics xngcpugraphics)