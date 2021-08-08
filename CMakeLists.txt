cmake_minimum_required(VERSION 3.19)

project(Watermelon)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)

find_package(SDL2 2.0.14 REQUIRED CONFIG)
find_package(SDL2_image 2.0.5 REQUIRED CONFIG)
find_package(box2d 2.4.1 REQUIRED CONFIG)

add_subdirectory(src)