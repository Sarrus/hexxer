cmake_minimum_required(VERSION 3.11)
project(hexxer C)

set(CMAKE_C_STANDARD 99)

link_libraries(pthread)
add_executable(hexxer main.c hexagon.c hexagon.h memory.c memory.h report.c report.h gpu.h)

if(DEFINED WITH_CUDA)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 14)
    target_sources(hexxer PRIVATE gpu.cu gpu.h memory.cuh)
    set_target_properties(hexxer PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            CUDA_ARCHITECTURES OFF)
    add_compile_definitions(WITH_CUDA)
endif()