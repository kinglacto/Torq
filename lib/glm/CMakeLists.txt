cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(GLM)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE include "${CMAKE_CURRENT_SOURCE_DIR}")

