cmake_minimum_required (VERSION 3.12)
project(factory-game VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm REQUIRED)
find_package(GTest REQUIRED)

add_executable(main
    src/main.cc
    src/draw.cc
    src/foundation.cc
    src/machine.cc
    src/state.cc
)
target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE glm::glm)

enable_testing()
add_executable(main_test test/main.cc)
target_link_libraries(main_test GTest::gtest_main)
add_test(NAME test COMMAND main_test)
