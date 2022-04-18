#pragma once

#include "SDL.h"

constexpr int score_width = 38, score_height = 50;

class ScoreBoard
{
public:
    ScoreBoard(const unsigned int &score, SDL_Renderer *renderer);
    ~ScoreBoard();
    void update();
    void draw();

private:
    float current_score;
    const unsigned int &score;
    SDL_Renderer *renderer;
    SDL_Texture *number;
};