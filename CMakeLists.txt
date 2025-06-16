cmake_minimum_required(VERSION 3.17)

project(Torq)
list(APPEND
    CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/glad)
add_subdirectory(lib/stb_image)

add_executable(torq
        main.cpp
        src/graphics/shader.cpp
        src/IO/keyboard.cpp
        src/IO/mouse.cpp
        src/IO/camera.cpp
        src/IO/screen.cpp
        src/graphics/mesh.cpp
        src/graphics/model.cpp
        src/graphics/material.cpp
        src/graphics/texture.cpp
        src/graphics/models/cube.cpp
        src/resources/resource.cpp
        src/resources/chunk_management.cpp
)

set_target_properties(torq
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)

include_directories(include)
target_link_libraries(torq
    ${GLFW3_LIBRARY}
    ${GLM_LIBRARY}
    OpenGL::GL
    glad
    stb_image
)
