cmake_minimum_required(VERSION 4.0)
project(factory_game)

set(CMAKE_CXX_STANDARD 17)

file(GLOB HEADER "src/*.h")
file(GLOB SOURCE "src/*.cc")

add_executable(factory_game ${HEADER} ${SOURCE})
target_include_directories(factory_game PRIVATE include)
