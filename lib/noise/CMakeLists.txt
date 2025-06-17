cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Noise)
add_library(noise INTERFACE)
target_include_directories(noise INTERFACE include "${CMAKE_CURRENT_SOURCE_DIR}/include")
