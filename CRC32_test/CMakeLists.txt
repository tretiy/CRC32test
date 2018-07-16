# CMakeList.txt : CMake project for CRC32_test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(../3rdparty/cxxopt)
# Add source to this project's executable.
add_executable (CRC32test "CRC32_test.cpp" "CRC32_test.h")

# TODO: Add tests and install targets if needed.
