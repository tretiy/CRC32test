# CMakeList.txt : CMake project for CRC32_test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(../3rdparty/cxxopt)
include_directories(../3rdparty/crc32c-master/include)
# Add source to this project's executable.
add_executable (CRC32test "CRC32_test.cpp" "CRC32_test.h")
target_link_libraries(CRC32test crc32c Threads::Threads)

