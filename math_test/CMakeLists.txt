cmake_minimum_required(VERSION 3.4)

project(math_test)

file(GLOB XNG_MATH_TEST_SRC_FILES *.cpp *.hpp)

include_directories(Eigen)

add_executable(math_test ${XNG_MATH_TEST_SRC_FILES})
target_link_libraries(math_test xngmath)