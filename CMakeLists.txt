cmake_minimum_required(VERSION 3.16)
project(soft_rasterization)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/stb)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCE_FILES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/renderer.cpp
    ${SOURCE_DIR}/mesh.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/texture.cpp
    ${SOURCE_DIR}/scene_renderer.cpp
    ${SOURCE_DIR}/platform/window.cpp
    ${SOURCE_DIR}/platform/desktop/glfw_window.cpp
)

add_executable(soft_rasterization ${SOURCE_FILES})
target_include_directories(soft_rasterization PRIVATE ${SOURCE_DIR})
target_link_libraries(soft_rasterization PRIVATE glfw glad::glad stb)
