cmake_minimum_required(VERSION 3.4)

project(xngfontbaker)

file(GLOB XNG_FONTBAKER_SOURCE_FILES *.cpp)
file(GLOB XNG_FONTBAKER_INCLUDE_FILES *.hpp *inl)

include_directories(${XNG_INCLUDE_DIRS})

add_executable(xngfontbaker  ${XNG_FONTBAKER_SOURCE_FILES} ${XNG_FONTBAKER_INCLUDE_FILES})
target_link_libraries(xngfontbaker ${XNG_LIBRARIES})

xng_set_modules_dependency(xngfontbaker)
xng_link_dependencies(xngfontbaker)