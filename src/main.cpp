#include "box2d/box2d.h"
#include "SDL.h"
#include "SDL_image.h"

#include <string>
#include <list>
#include <random>
#include <array>
#include <tuple>

#include "config.h"

b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);

std::list<b2Body *> wait_delete;
std::list<std::tuple<float, float, float>> wait_add;

bool is_run = false;

unsigned int score = 0;

class MyContactListener : public b2ContactListener
{
    virtual void BeginContact(b2Contact *contact) override
    {

        b2CircleShape *A = dynamic_cast<b2CircleShape *>(contact->GetFixtureA()->GetShape());
        b2CircleShape *B = dynamic_cast<b2CircleShape *>(contact->GetFixtureB()->GetShape());

        if (A != nullptr && B != nullptr && A->m_radius == B->m_radius)
        {
            auto body_A = contact->GetFixtureA()->GetBody();
            auto body_B = contact->GetFixtureB()->GetBody();
            std::size_t index = 0;
            for (index = 0; index < balls_radius.size(); index++)
            {
                if (balls_radius.at(index) == A->m_radius)
                {
                    break;
                }
            }
            if (index + 1 == balls_radius.size())
            {
                is_run = false;
            }
            score += static_cast<unsigned int>(A->m_radius);
            wait_delete.push_back(body_A);
            wait_delete.push_back(body_B);

            wait_add.emplace_back((body_A->GetPosition().x + body_B->GetPosition().x) / 2, (body_A->GetPosition().y + body_B->GetPosition().y) / 2, balls_radius.at(index + 1));
        }
        return;
    }
    virtual void EndContact(b2Contact *contact)
    {
        B2_NOT_USED(contact);
    }
    virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
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

MyContactListener m;

float radius = 0.0f;
int next = -1;

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<std::size_t> distr(0, 4);

std::array<SDL_Texture *, 11> textures;
SDL_Texture *ground;
SDL_Texture *number;

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("Watermelon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) != 0)
    {
        SDL_Log("Unable to set the blend mode: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        return false;
    }
    is_run = true;
    return true;
}

inline SDL_FPoint CoordinateMapping(float x, float y)
{
    return {x, height - y};
}

inline void gen_next_ball()
{
    next = distr(eng);
    radius = balls_radius.at(next);
}

// x, y: world position
void add_ball(float x, float y, float radius)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.linearVelocity = b2Vec2(0, -50000.0f);
    b2Body *body = world.CreateBody(&bodyDef);

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 3.0f;
    fixtureDef.friction = 0.6f;
    fixtureDef.restitution = 0.1f;

    body->CreateFixture(&fixtureDef);
}

void place_blocks()
{
    b2BodyDef groundBodyDef;
    b2Body *body;
    b2PolygonShape groundBox;

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
                // check x
                if (event.button.x < radius)
                {
                    add_ball(radius, height - 100.0f, radius);
                }
                else if (event.button.x > width - right_block_width - radius)
                {
                    add_ball(width - radius, height - 100.0f, radius);
                }
                else
                {
                    add_ball(event.button.x, height - 100.0f, radius);
                }
                gen_next_ball();
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

    // draw next ball
    if (next >= 0)
    {
        SDL_FRect dst{width / 2, 100.0f - radius, 2 * radius, 2 * radius};
        SDL_RenderCopyF(renderer, textures.at(next), nullptr, &dst);
    }

    // draw score

    // draw balls
    auto e = world.GetBodyList();
    while (e != nullptr)
    {
        if (dynamic_cast<b2CircleShape *>(e->GetFixtureList()->GetShape()) != nullptr)
        {
            SDL_FPoint center = CoordinateMapping(e->GetPosition().x, e->GetPosition().y);
            float radius = static_cast<b2CircleShape *>(e->GetFixtureList()->GetShape())->m_radius;
            SDL_FRect dst{center.x - radius, center.y - radius, 2 * radius, 2 * radius};
            std::size_t index = 0;
            for (index = 0; index < balls_radius.size(); index++)
            {
                if (balls_radius.at(index) == radius)
                {
                    break;
                }
            }
            SDL_RenderCopyExF(renderer, textures.at(index), nullptr, &dst, e->GetAngle() * 360.0f / (2 * std::_Pi), nullptr, SDL_FLIP_NONE);
        }
        e = e->GetNext();
    }
    SDL_RenderPresent(renderer);
}

void init_texture()
{
    for (std::size_t i = 0; i < textures.size(); i++)
    {
        textures.at(i) = IMG_LoadTexture(renderer, ("../assets/" + std::to_string(i) + ".png").c_str());
        if (textures.at(i) == nullptr)
        {
            SDL_Log("Error at load texture: %s", SDL_GetError());
        }
    }
    ground = IMG_LoadTexture(renderer, "../assets/ground.png");
    number = IMG_LoadTexture(renderer, "../assets/number.png");
}

void update_world()
{
    for (auto e : wait_delete)
    {
        world.DestroyBody(e);
    }
    wait_delete.clear();
    for (auto [x, y, r] : wait_add)
    {
        add_ball(x, y, r);
    }
    wait_add.clear();
}

int main(int argc, char *argv[])
{
    if (init() == false)
    {
        SDL_Log("Failed to init");
        SDL_Quit();
        return -1;
    }
    world.SetContactListener(&m);
    init_texture();
    place_blocks();
    gen_next_ball();
    while (is_run)
    {
        ticks = SDL_GetTicks();

        world.Step(time_step, 6, 2);

        update_world();
        handle_event();

        draw();

        if (ticks_used != SDL_GetTicks() - ticks)
        {
            ticks_used = SDL_GetTicks() - ticks;
            SDL_SetWindowTitle(window, ("Watermelon    [ticks used: " + std::to_string(ticks_used) + "]").c_str());
        }
        while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticks + frame_ticks))
            ;
    }

    for (auto e : textures)
    {
        SDL_DestroyTexture(e);
    }
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(number);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}