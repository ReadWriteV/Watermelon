#pragma once

#include "SDL.h"
#include "asset_manager.h"

constexpr int score_width = 38, score_height = 50;

class ScoreBoard
{
public:
    ScoreBoard(const unsigned int &score, SDL_Renderer *renderer, const AssetManager &asset_manager);
    ~ScoreBoard();
    void update();
    void draw();

private:
    const AssetManager &asset_manager;
    float current_score;
    const unsigned int &score;
    SDL_Renderer *renderer;
};