#include "box2d/box2d.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include <string>
#include <list>
#include <random>
#include <array>
#include <tuple>
#include <limits>
#include <algorithm>
#include <cassert>

#include "config.h"

#include "asset_manager.h"
#include "scoreboard.h"

b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);

b2Body *last_fruit = nullptr;

std::list<b2Body *> wait_delete;
std::list<std::tuple<float, float, std::size_t>> wait_add;

bool is_run = false;

unsigned int score = 0;

class MyContactListener : public b2ContactListener
{
    virtual void BeginContact(b2Contact *contact) override
    {

        b2Body *A = contact->GetFixtureA()->GetBody();
        b2Body *B = contact->GetFixtureB()->GetBody();

        if (A->GetUserData().pointer == B->GetUserData().pointer && A->GetUserData().pointer != std::numeric_limits<uintptr_t>::max())
        {
            auto index = A->GetUserData().pointer;
            score += static_cast<unsigned int>(index + 1);

            if (index + 2 == fruits_radius.size())
            {
                is_run = false;
                SDL_Log("win !!!, score = %d", score);
            }
            wait_delete.push_back(A);
            wait_delete.push_back(B);

            wait_add.emplace_back((A->GetPosition().x + B->GetPosition().x) / 2, (A->GetPosition().y + B->GetPosition().y) / 2, index + 1);
        }
        return;
    }
    virtual void EndContact(b2Contact *contact) override
    {
        B2_NOT_USED(contact);
    }
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override
    {
        B2_NOT_USED(contact);
        B2_NOT_USED(oldManifold);
    }
    virtual void PostSolve(const b2Contact *contact, const b2ContactImpulse *impulse)
    {
        B2_NOT_USED(contact);
        B2_NOT_USED(impulse);
    }
};

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
Uint32 ticks = 0, ticks_used = 0;

float radius = 0.0f;
std::size_t next = std::numeric_limits<std::size_t>::max();

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<std::size_t> distr(0, 4);

AssetManager asset_manager("../assets");

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("Watermelon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0)
    {
        SDL_Log("Unable to set the blend mode: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return false;
    }

    // Audio Init
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        SDL_Log("Unable to initialize MIX: %s", Mix_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        SDL_Log("Unable to initialize Audio: %s", Mix_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        SDL_Quit();
        return false;
    }
    is_run = true;
    return true;
}

inline SDL_FPoint CoordinateMapping(float x, float y)
{
    return {x, height - y};
}

inline void gen_next_fruit()
{
    next = distr(eng);
    radius = fruits_radius.at(next);
}

// x, y: world position
void add_fruit(float x, float y, float radius, std::size_t index = std::numeric_limits<uintptr_t>::max())
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.angularDamping = 0.7f;
    bodyDef.linearVelocity = b2Vec2(0, -100.0f);
    bodyDef.userData.pointer = index;
    last_fruit = world.CreateBody(&bodyDef);

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 6.0f;
    fixtureDef.friction = 0.9f;
    fixtureDef.restitution = 0.1f;

    last_fruit->CreateFixture(&fixtureDef);
}

void place_blocks()
{
    b2BodyDef groundBodyDef;
    b2Body *body;
    b2PolygonShape groundBox;

    groundBodyDef.userData.pointer = std::numeric_limits<uintptr_t>::max();

    groundBodyDef.position.Set(left_block_center_x, left_block_center_y);
    body = world.CreateBody(&groundBodyDef);
    groundBox.SetAsBox(left_block_width / 2, left_block_height / 2);
    body->CreateFixture(&groundBox, 0.0f);

    groundBodyDef.position.Set(right_block_center_x, right_block_center_y);
    body = world.CreateBody(&groundBodyDef);
    groundBox.SetAsBox(right_block_width / 2, right_block_height / 2);
    body->CreateFixture(&groundBox, 0.0f);

    groundBodyDef.position.Set(down_block_center_x, down_block_center_y);
    body = world.CreateBody(&groundBodyDef);
    groundBox.SetAsBox(down_block_width / 2, down_block_height / 2);
    body->CreateFixture(&groundBox, 0.0f);
}

void handle_event()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            is_run = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (last_fruit != nullptr)
                {
                    if (last_fruit->GetPosition().y + fruits_radius.at(last_fruit->GetUserData().pointer) > red_line_height)
                    {
                        break;
                    }
                }
                float x = event.button.x;
                // check x
                x = std::clamp(x, radius, width - radius);
                add_fruit(x, fruit_y, radius, next);
                gen_next_fruit();
            }
            break;
        default:
            break;
        }
    }
}

void draw()
{
    // draw blocks
    SDL_SetRenderDrawColor(renderer, 117, 85, 60, SDL_ALPHA_OPAQUE);

    SDL_FPoint upper_left = CoordinateMapping(down_block_center_x - down_block_width / 2, down_block_center_y + down_block_height / 2);
    SDL_FRect down{upper_left.x, upper_left.y, down_block_width, down_block_height};
    SDL_RenderFillRectF(renderer, &down);

    // clear with background
    SDL_SetRenderDrawColor(renderer, 254, 232, 159, SDL_ALPHA_OPAQUE);
    SDL_FRect background{0.0f, 0.0f, down_block_width, height - down_block_height};
    SDL_RenderFillRectF(renderer, &background);

    // draw next fruit
    if (next != std::numeric_limits<std::size_t>::max())
    {
        SDL_FRect dst{width / 2 - radius, 70.0f - radius, 2 * radius, 2 * radius};
        SDL_RenderCopyF(renderer, asset_manager.get_fruit_texture(next), nullptr, &dst);
    }

    // draw red line
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, 0, height - red_line_height, width, height - red_line_height);

    // draw fruits
    auto e = world.GetBodyList();
    while (e != nullptr)
    {
        if (e->GetUserData().pointer != std::numeric_limits<std::size_t>::max())
        {
            auto index = e->GetUserData().pointer;
            auto radius = fruits_radius.at(index);
            SDL_FPoint center = CoordinateMapping(e->GetPosition().x, e->GetPosition().y);
            SDL_FRect dst{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
            SDL_RenderCopyExF(renderer, asset_manager.get_fruit_texture(index), nullptr, &dst, e->GetAngle() * 180.0f / std::_Pi, nullptr, SDL_FLIP_NONE);
        }
        e = e->GetNext();
    }
}

void update_world()
{
    bool need_play_music = false;
    if (wait_add.size() != 0)
    {
        need_play_music = true;
    }
    for (auto e : wait_delete)
    {
        world.DestroyBody(e);
    }
    wait_delete.clear();
    for (const auto [x, y, i] : wait_add)
    {
        add_fruit(x, y, fruits_radius.at(i), i);
    }
    wait_add.clear();
    if (need_play_music)
    {
        Mix_PlayMusic(asset_manager.get_combine_music(), 1);
    }
}

void check_height()
{
    float max_height = 0;
    auto e = world.GetBodyList();
    while (e != nullptr)
    {
        if (e->GetUserData().pointer != std::numeric_limits<std::size_t>::max() && e->GetLinearVelocity().y == 0)
        {
            max_height = std::max(max_height, e->GetPosition().y + fruits_radius.at(e->GetUserData().pointer));
        }
        e = e->GetNext();
    }
    // SDL_Log("%f", max_height);
}

int main(int argc, char *argv[])
{
    if (init() == false)
    {
        SDL_Log("Failed to init");
        return -1;
    }

    MyContactListener m;
    world.SetContactListener(&m);

    if (asset_manager.load_assets(renderer) == false)
    {
        Mix_CloseAudio();
        Mix_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    ScoreBoard score_board(score, renderer, asset_manager);

    place_blocks();
    gen_next_fruit();
    while (is_run)
    {
        ticks = SDL_GetTicks();

        world.Step(time_step, 6, 2);

        update_world();

        score_board.update();

        check_height();
        handle_event();

        draw();

        // draw score
        score_board.draw();

        SDL_RenderPresent(renderer);

        if (ticks_used != SDL_GetTicks() - ticks)
        {
            ticks_used = SDL_GetTicks() - ticks;
            SDL_SetWindowTitle(window, ("Watermelon    [ticks used: " + std::to_string(ticks_used) + "]").c_str());
        }
        while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticks + frame_ticks))
            ;
    }

    Mix_CloseAudio();
    Mix_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
