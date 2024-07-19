cmake_minimum_required(VERSION 3.20)
project(HydraTest)

set(SOURCES
    "src/main.cpp"
    "src/SceneIndexDemo.cpp"
)
add_executable(HydraTest ${SOURCES})
target_include_directories(HydraTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(USD_ROOT "/home/teachmain/usd")
set(pxr_DIR ${USD_ROOT})
find_package(pxr REQUIRED)
target_include_directories(HydraTest PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(HydraTest PUBLIC ${PXR_LIBRARIES})
message(STATUS "Found USD include path in : ${PXR_INCLUDE_DIRS}")

    
