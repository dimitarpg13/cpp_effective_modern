cmake_minimum_required(VERSION 3.20)

# set the project name
project(Auto VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(prefer_auto_to_explicit_type01 prefer_auto_to_explicit_type01.cpp)
