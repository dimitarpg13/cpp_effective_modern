cmake_minimum_required(VERSION 3.20)

# set the project name
project(TemplateTypeDeduction VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(template_type_deduction01 template_type_deduction01.cpp)
add_executable(template_type_deduction02 template_type_deduction02.cpp)
add_executable(auto_type_deduction01 auto_type_deduction01.cpp)
add_executable(understand_decltype01 understand_decltype01.cpp)
