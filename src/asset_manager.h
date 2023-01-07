#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include <array>
#include <filesystem>

class AssetManager
{
public:
    AssetManager(const std::filesystem::path &assets_path);
    ~AssetManager();

    bool load_assets(SDL_Renderer *renderer);
    SDL_Texture *get_fruit_texture(const std::size_t index) const;
    SDL_Texture *get_ground_texture() const;
    SDL_Texture *get_numbers_texture() const;
    Mix_Music *get_combine_music() const;

private:
    std::filesystem::path assets_path;

    std::array<SDL_Texture *, 11> fruits;
    SDL_Texture *numbers;

    Mix_Music *music;
};