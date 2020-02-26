cmake_minimum_required(VERSION 3.15)
project(projet)

set(CMAKE_CXX_STANDARD 14)
set( CMAKE_CXX_FLAGS "-O3 " )

add_executable(projet main.cpp Objet.h Objet.hpp)