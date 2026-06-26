cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(soft_rasterization)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(SOURCE_FILES 
    ${SOURCE_DIR}/main.cpp 
    ${SOURCE_DIR}/renderer.cpp
    ${SOURCE_DIR}/mesh.cpp
    ${SOURCE_DIR}/camera.cpp
    )
add_executable(soft_rasterization ${SOURCE_FILES})