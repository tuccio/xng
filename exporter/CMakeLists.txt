cmake_minimum_required(VERSION 3.4)

project(xngexporter)

file(GLOB XNG_EXPORTER_SOURCE_FILES *.cpp)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngexporter SHARED ${XNG_EXPORTER_SOURCE_FILES} ${XNG_CORE_INCLUDE_FILES})
target_link_libraries(xngexporter ${XNG_EXPORTER_LIBRARIES})