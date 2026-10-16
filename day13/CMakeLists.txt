cmake_minimum_required(VERSION 3.20)
project(day13_strings CXX)

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

add_library(day13_strings_lib INTERFACE)
target_include_directories(day13_strings_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(bench_strings bench/bench_strings.cpp)
target_link_libraries(bench_strings PRIVATE benchmark::benchmark day13_strings_lib)

add_executable(sso_demo demo/sso_demo.cpp)
target_link_libraries(sso_demo PRIVATE day13_strings_lib)
