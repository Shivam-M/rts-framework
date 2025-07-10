cmake_minimum_required(VERSION 3.10)
project(rts_framework)

cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries)

include_directories(
    include
    include/freetype
    include/GL
    include/GLFW
    include/glm
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    assets/*.cpp
    io/*.cpp
    tools/*.cpp
    *.cpp
)

list(FILTER SOURCES EXCLUDE REGEX "/build/")

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    assets/*.h
    io/*.h
    tools/*.h
    include/**/*.h
    *.h
)

# file(GLOB SHADERS shaders/*.vert shaders/*.frag)
# file(GLOB DATA data/)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GLEW REQUIRED)
find_package(fmt REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    Freetype::Freetype
    fmt::fmt
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)
