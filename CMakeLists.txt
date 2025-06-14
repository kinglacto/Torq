cmake_minimum_required(VERSION 3.17)

project(OpenGL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(OpenGL 
        glad.c 
        main.cpp
        src/graphics/shader.cpp
        src/IO/keyboard.cpp
        src/IO/mouse.cpp
        src/IO/camera.cpp
        src/IO/screen.cpp
        src/graphics/mesh.cpp
        src/graphics/model.cpp
        src/graphics/models/sphere.cpp
        src/graphics/texture.cpp
        src/graphics/stb.cpp
        src/graphics/models/cube.cpp
        src/resources/resource.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(include)
target_link_libraries(OpenGL GL dl glfw)