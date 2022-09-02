#include "scoreboard.h"

#include <algorithm>
#include <cassert>

ScoreBoard::ScoreBoard(const unsigned int &score, SDL_Renderer *renderer, const AssetManager &asset_manager)
    : current_score(score), score(score), renderer(renderer), asset_manager(asset_manager)
{
}

ScoreBoard::~ScoreBoard()
{
}

void ScoreBoard::update()
{
    current_score += (score - current_score) / 10;
    current_score = std::min(static_cast<float>(score), current_score);
}

void ScoreBoard::draw()
{
    int h = current_score / 100, m = (static_cast<int>(current_score) % 100) / 10, l = static_cast<int>(current_score) % 10;
    SDL_Rect src;
    SDL_Rect dst;
    if (h != 0)
    {
        src = {h * 76, 0, 76, 100};
        dst = {0, 0, score_width, score_height};
        SDL_RenderCopy(renderer, asset_manager.get_numbers_texture(), &src, &dst);
    }
    if (m != 0 || h != 0)
    {
        src = {m * 76, 0, 76, 100};
        dst = {score_width, 0, score_width, score_height};
        SDL_RenderCopy(renderer, asset_manager.get_numbers_texture(), &src, &dst);
    }
    src = {l * 76, 0, 76, 100};
    dst = {score_width * 2, 0, score_width, score_height};
    SDL_RenderCopy(renderer, asset_manager.get_numbers_texture(), &src, &dst);
}
