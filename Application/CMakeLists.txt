cmake_minimum_required(VERSION 3.10)
project(NetShare)

set(CMAKE_CXX_STANDARD 17)

add_executable(NetShare main.cpp POSIX/POSIX.cpp WIN32/WIN32.cpp)