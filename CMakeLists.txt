cmake_minimum_required(VERSION 3.20)
project(HydraTest)

set(SOURCES
    "src/main.cpp"
)
add_executable(HydraTest ${SOURCES})
target_include_directories(HydraTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(glfw3 REQUIRED)
target_include_directories(HydraTest PUBLIC ${glfw3_INCLUDE_DIRS})
target_link_libraries(HydraTest PUBLIC ${glfw3_LIBRARIES})


set(USD_ROOT "/home/teachmain/usd")
set(pxr_DIR ${USD_ROOT})
find_package(pxr REQUIRED)
target_include_directories(HydraTest PUBLIC ${PXR_INCLUDE_DIRS})
target_link_libraries(HydraTest PUBLIC ${PXR_LIBRARIES})
message(STATUS "Found USD include path in : ${PXR_INCLUDE_DIRS}")

add_library(TriSceneIndex src/TriSceneIndex.cpp)
target_include_directories(TriSceneIndex PUBLIC ${PXR_INCLUDE_DIRS})
target_include_directories(TriSceneIndex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(TriSceneIndex PUBLIC ${PXR_LIBRARIES})

target_link_libraries(HydraTest PUBLIC TriSceneIndex)