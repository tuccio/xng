cmake_minimum_required(VERSION 3.4)

project(xngvktests)

option(XNG_VK_TESTS "Build Vulkan tests." ON)

if (XNG_USE_VULKAN AND XNG_VK_TESTS)

    find_package(wxWidgets COMPONENTS core base aui REQUIRED)
    
    add_definitions(-DUNICODE -D_UNICODE)
    include(${wxWidgets_USE_FILE})
    
    file(GLOB XNG_VKTESTS_SOURCE_FILES *.cpp)
    file(GLOB XNG_VKTESTS_INCLUDE_FILES *.hpp *inl)
    
    include_directories(${XNG_INCLUDE_DIRS})
    
    add_library(xngvktests SHARED ${XNG_VKTESTS_SOURCE_FILES} ${XNG_VKTESTS_INCLUDE_FILES})
    xng_link_module(xngvktests)
    
    target_link_libraries(xngvktests ${XNG_LIBRARIES} ${wxWidgets_LIBRARIES} xngvk)
    xng_link_dependencies(xngvktests)

endif ()