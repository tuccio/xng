cmake_minimum_required(VERSION 3.4)
project(xngcugraphics)

find_package(CUDA)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)

if (MSVC)
	add_definitions(-DXNG_MSVC)
endif()

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "-gencode;arch=compute_30,code=sm_30; -gencode;arch=compute_35,code=sm_35; -gencode;arch=compute_35,code=compute_35; -gencode;arch=compute_20,code=sm_20;")
set(CUDA_NVCC_FLAGS_DEBUG ${CUDA_NVCC_FLAGS_DEBUG} "-G")
	
include_directories(${CUDA_INCLUDE_DIRS})

file(GLOB XNG_CUGRAPHICS_SOURCE_FILES *.cpp *.cu)
file(GLOB XNG_CUGRAPHICS_INCLUDE_FILES *.cuh *.hpp *.inl *.h)

cuda_add_library(xngcugraphics SHARED ${XNG_CUGRAPHICS_SOURCE_FILES} ${XNG_CUGRAPHICS_INCLUDE_FILES})