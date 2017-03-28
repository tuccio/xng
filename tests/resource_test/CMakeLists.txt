cmake_minimum_required(VERSION 3.4)

project(resource_test)

file(GLOB XNG_RESOURCE_TEST_SOURCE_FILES *.cpp)
file(GLOB XNG_RESOURCE_TEST_INCLUDE_FILES *.hpp *inl)

add_executable(resource_test ${XNG_RESOURCE_TEST_SOURCE_FILES} ${XNG_RESOURCE_TEST_INCLUDE_FILES})

target_link_libraries(resource_test xngcore xngres)