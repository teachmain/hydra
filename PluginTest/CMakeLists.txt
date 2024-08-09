cmake_minimum_required(VERSION 3.20)
project(HdPluginTest)

set(SOURCES
    PluginFinder.cpp
)
add_executable(HdPluginFinder ${SOURCES})

set(USD_ROOT "/home/teachmain/usd")
set(pxr_DIR ${USD_ROOT})
find_package(pxr REQUIRED)
target_include_directories(HdPluginFinder PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(HdPluginFinder PUBLIC ${PXR_LIBRARIES})