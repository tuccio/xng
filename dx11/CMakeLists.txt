cmake_minimum_required(VERSION 3.4)

project(xngdx11)

if (XNG_USE_DX11)

	file(GLOB XNG_DX11_SOURCE_FILES *.cpp)
	file(GLOB XNG_DX11_INCLUDE_FILES include/xng/*.hpp include/xng/*.inl include/xng/dx11/*.hpp include/xng/dx11/*.inl)
	
	add_library(xngdx11 STATIC ${XNG_DX11_SOURCE_FILES} ${XNG_DX11_INCLUDE_FILES})
	
endif ()