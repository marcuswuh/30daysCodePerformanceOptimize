cmake_minimum_required(VERSION 3.20)
project(day6_memory CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

add_compile_options(-Wall -Wextra -fno-omit-frame-pointer -g)

add_library(day6_match INTERFACE)
target_include_directories(day6_match INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(mem_demo demo/mem_demo.cpp)
target_link_libraries(mem_demo PRIVATE day6_match)

# alloc_counter.cpp 必须直接链进可执行文件，否则 static lib 中的 operator new 可能被链接器丢弃
add_executable(alloc_trace_demo demo/alloc_trace_demo.cpp src/alloc_counter.cpp)
target_link_libraries(alloc_trace_demo PRIVATE day6_match)
if(APPLE)
  target_link_options(alloc_trace_demo PRIVATE "-Wl,-interposable")
endif()
