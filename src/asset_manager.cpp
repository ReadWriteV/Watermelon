#include "asset_manager.h"

#include <cassert>
#include <string>

AssetManager::AssetManager(const std::filesystem::path &assets_path) : assets_path(assets_path)
{
}

AssetManager::~AssetManager()
{
    for (auto e : fruits)
    {
        SDL_DestroyTexture(e);
    }
    SDL_DestroyTexture(numbers);
    Mix_FreeMusic(music);
}

bool AssetManager::load_assets(SDL_Renderer *renderer)
{
    if (std::filesystem::exists(assets_path) == false)
    {
        SDL_Log("Assets path [%s] does not exist!", std::filesystem::absolute(assets_path).c_str());
        return false;
    }
    for (std::size_t i = 0; i < fruits.size(); i++)
    {
        fruits.at(i) = IMG_LoadTexture(renderer, (assets_path / std::filesystem::path(std::to_string(i) + ".png")).u8string().c_str());
        assert(fruits.at(i) != nullptr);
    }
    numbers = IMG_LoadTexture(renderer, (assets_path / std::filesystem::path("number.png")).u8string().c_str());
    assert(numbers != nullptr);
    music = Mix_LoadMUS((assets_path / std::filesystem::path("combine.mp3")).u8string().c_str());
    assert(music != nullptr);
    return true;
}

SDL_Texture *AssetManager::get_fruit_texture(const std::size_t index) const
{
    return fruits.at(index);
}
SDL_Texture *AssetManager::get_numbers_texture() const
{
    return numbers;
}
Mix_Music *AssetManager::get_combine_music() const
{
    return music;
}