cmake_minimum_required(VERSION 3.4)

project(xngeditor)

find_package(wxWidgets COMPONENTS core base aui REQUIRED)

add_definitions(-DUNICODE -D_UNICODE)
include(${wxWidgets_USE_FILE})

file(GLOB XNG_EDITOR_SOURCE_FILES *.cpp)
file(GLOB XNG_EDITOR_INCLUDE_FILES *.hpp *inl)

include_directories(${XNG_INCLUDE_DIRS})

add_library(xngeditor SHARED ${XNG_EDITOR_SOURCE_FILES} ${XNG_EDITOR_INCLUDE_FILES})

target_link_libraries(xngeditor ${XNG_LIBRARIES} ${wxWidgets_LIBRARIES})