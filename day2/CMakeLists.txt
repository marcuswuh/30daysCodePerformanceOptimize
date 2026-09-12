cmake_minimum_required(VERSION 3.20)
project(day2_benchmark CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

add_compile_options(-Wall -Wextra -fno-omit-frame-pointer)

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

add_library(day2_sort src/sort/sort.cpp)
target_include_directories(day2_sort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sort)

add_executable(bench_dce src/bench_dce.cpp)
target_link_libraries(bench_dce PRIVATE benchmark::benchmark)

add_executable(bench_sort_case src/bench_sort_case.cpp)
target_link_libraries(bench_sort_case PRIVATE benchmark::benchmark day2_sort)
