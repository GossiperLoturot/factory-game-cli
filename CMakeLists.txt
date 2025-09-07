cmake_minimum_required(VERSION 4.0)
project(factory)

set(CMAKE_CXX_STANDARD 17)

add_executable(factory main.cpp
    draw.h draw.cpp
    foundation.h foundation.cpp
    inout.h inout.cpp
    machine.h machine.cpp
    pipe.h pipe.cpp
    pool.h
    state.h state.cpp)
