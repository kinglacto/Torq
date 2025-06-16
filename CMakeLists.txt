cmake_minimum_required(VERSION 3.17)

project(Torq)
list(APPEND
    CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glad)
add_subdirectory(lib/glm)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/glfw)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(torq
        main.cpp
        ${MY_SOURCES}
)

set_target_properties(torq
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

find_package(OpenGL REQUIRED)

include_directories(include)
target_link_libraries(torq
    OpenGL::GL
    glfw
    glad
    glm
    stb_image
)
