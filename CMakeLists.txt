cmake_minimum_required(VERSION 3.11)
project(hexxer C)

set(CMAKE_C_STANDARD 99)

link_libraries(pthread)
add_executable(hexxer main.c hexagon.c hexagon.h)