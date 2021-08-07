cmake_minimum_required(VERSION 3.20)

# set the project name
project(EffectiveModernC++ VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(deducing_types)
add_subdirectory(universal_references)
add_subdirectory(using_noexcept)
add_subdirectory(auto)
