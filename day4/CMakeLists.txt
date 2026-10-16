cmake_minimum_required(VERSION 3.20)
project(day4_perf_record CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

add_compile_options(-Wall -Wextra -fno-omit-frame-pointer -g)

add_library(day4_workload src/workload.cpp)
target_include_directories(day4_workload PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(workload_demo demo/workload_demo.cpp)
target_link_libraries(workload_demo PRIVATE day4_workload)
