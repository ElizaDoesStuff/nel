cmake_minimum_required(VERSION 3.30)
project(nel)

add_executable(nel Source/main.cpp)
target_link_libraries(nel -libstdc++ -libgcc)
