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
add_subdirectory(lib/noise)

if (BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug Mode")
    add_definitions(-DDEBUG_MODE)
elseif(BUILD_TYPE STREQUAL "Test")
    message(STATUS "Building in Test mode")
    add_definitions(-DTEST_MODE)
else()
    message(STATUS "Building in Release mode")
    add_definitions(-DRELEASE_MODE)
endif()

file(MAKE_DIRECTORY cache/chunks)
file(MAKE_DIRECTORY cache/maps)

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
find_package(ZLIB REQUIRED)

target_include_directories(torq PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(torq PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_definitions(torq PRIVATE CACHE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/cache")
target_compile_definitions(torq PRIVATE CHUNKS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/chunks")
target_link_libraries(torq PRIVATE
    OpenGL::GL
    glfw
    glad
    glm
    stb_image
    ZLIB::ZLIB
    noise
)
