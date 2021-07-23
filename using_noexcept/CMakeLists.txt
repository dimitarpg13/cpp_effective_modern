cmake_minimum_required(VERSION 3.20)

# set the project name
project(UsingNoExcept VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(using_noexcept01 using_noexcept01.cpp)
