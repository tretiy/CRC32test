﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("CRC32_test" C CXX)

set (CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(CRC32C_BUILD_BENCHMARKS OFF)
option(CRC32C_BUILD_TESTS OFF)
option(CRC32C_USE_GLOG OFF)
# Include sub-projects.
add_subdirectory("3rdparty")
add_subdirectory("CRC32_test")
