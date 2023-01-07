cmake_minimum_required(VERSION 3.19)

project(Watermelon CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(box2d REQUIRED)

add_subdirectory(src)