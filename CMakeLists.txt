cmake_minimum_required(VERSION 3.19)

project(Watermelon)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

find_package(SDL2 2.0.14 REQUIRED)
find_package(SDL2_image 2.0.5 REQUIRED)
find_package(SDL2_mixer 2.0.4 REQUIRED)
find_package(box2d 2.4.1 REQUIRED)

add_subdirectory(src)