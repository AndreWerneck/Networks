# cmake_minimum_required(VERSION <specify CMake version here>)
project(tp1Desk)

set(CMAKE_CXX_STANDARD 14)

add_executable(server servidor.cpp common.cpp common.h)
add_executable(cliente cliente.cpp common.cpp common.h)