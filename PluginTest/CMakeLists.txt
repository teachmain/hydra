cmake_minimum_required(VERSION 3.20)
project(HdPluginTest)

set(SOURCES
    PluginFinder.cpp
)
add_executable(HdPluginFinder ${SOURCES})
add_executable(HdTinyTest testHdTiny.cpp)
add_library(TriSceneIndex TriSceneIndex.cpp)

set(USD_ROOT "/home/teachmain/usd")
set(pxr_DIR ${USD_ROOT})
find_package(pxr REQUIRED)

target_include_directories(HdPluginFinder PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(HdPluginFinder PUBLIC ${PXR_LIBRARIES})

target_include_directories(HdTinyTest PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(HdTinyTest PUBLIC ${PXR_LIBRARIES})

target_include_directories(TriSceneIndex PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(TriSceneIndex PUBLIC ${PXR_LIBRARIES})