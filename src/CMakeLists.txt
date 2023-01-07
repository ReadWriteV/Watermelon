add_executable(${PROJECT_NAME} main.cpp scoreboard.cpp asset_manager.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE box2d::box2d SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer)
