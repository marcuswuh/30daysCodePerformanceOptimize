cmake_minimum_required(VERSION 3.20)
project(day14_capstone CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

add_compile_options(-Wall -Wextra -fno-omit-frame-pointer -g)

find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
  include(FetchContent)
  set(BENCHMARK_ENABLE_TESTING OFF)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
  FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1)
  FetchContent_MakeAvailable(benchmark)
endif()

add_library(day14_blur src/blur.cpp)
target_include_directories(day14_blur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(blur_demo demo/blur_demo.cpp)
target_link_libraries(blur_demo PRIVATE day14_blur)

add_executable(bench_blur bench/bench_blur.cpp)
target_link_libraries(bench_blur PRIVATE benchmark::benchmark day14_blur)
