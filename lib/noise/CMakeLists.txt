cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(noise)
add_library(noise)
target_sources(noise PRIVATE src/Noise.cpp)
target_include_directories(noise PUBLIC include)
