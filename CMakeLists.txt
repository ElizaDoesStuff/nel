cmake_minimum_required(VERSION 3.30)
project(nel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(Dependencies/glad)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(Dependencies/glfw)

add_executable(nel Source/main.cpp)
target_link_libraries(nel glad glfw -libstdc++ -libgcc)