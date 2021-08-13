#pragma once

#include <array>

constexpr float frame_rate{120.0f};
constexpr float world_rate{30.0f};
constexpr float time_step{1.0f / world_rate};

constexpr unsigned int frame_ticks{static_cast<unsigned int>(1000.0f / frame_rate)};

constexpr int width{427};
constexpr int height{754};

constexpr std::array<float, 11> fruits_radius{{20.0f, 25.0f, 30.0f, 35.0f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 65.0f, 70.0f}};

constexpr float left_block_width = 10.0f;
constexpr float left_block_height = height;

constexpr float right_block_width = 10.0f;
constexpr float right_block_height = height;

constexpr float down_block_width = width;
constexpr float down_block_height = 20.0f;

constexpr float left_block_center_x = -left_block_width / 2;
constexpr float left_block_center_y = left_block_height / 2;

constexpr float right_block_center_x = down_block_width + right_block_width / 2;
constexpr float right_block_center_y = right_block_height / 2;

constexpr float down_block_center_x = down_block_width / 2;
constexpr float down_block_center_y = down_block_height / 2;

constexpr int score_width = 38, score_height = 50;
