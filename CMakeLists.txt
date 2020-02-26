cmake_minimum_required(VERSION 3.15)
project(projet)

set(CMAKE_CXX_STANDARD 14)
set( CMAKE_CXX_FLAGS "-O3 " )
include_directories(lib/stb)

add_executable(projet main.cpp)