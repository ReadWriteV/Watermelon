add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} box2d::box2d SDL2::SDL2 SDL2::SDL2main SDL2_image)
