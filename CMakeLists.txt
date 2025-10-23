cmake_minimum_required(VERSION 3.10)
project(rts_framework)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "*.cpp" "*.c")
list(FILTER SOURCES EXCLUDE REGEX "/build/")

# TODO: copy in the future instead of running from parent directory
# file(GLOB SHADERS shaders/*.vert shaders/*.frag)
# file(GLOB DATA data/)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    ${GLFW_LIBRARIES}
    Freetype::Freetype
    fmt::fmt
)