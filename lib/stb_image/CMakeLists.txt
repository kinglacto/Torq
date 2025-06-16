cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(stb_image)
add_library(stb_image)
target_sources(stb_image PRIVATE src/stb_image.cpp)
target_include_directories(stb_image PUBLIC include)

