#pragma once

#include "vector.hpp"
#include <SDL2/SDL.h>
#include <string>

typedef enum Direction {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
} Direction;

class Actor {
    public:
        Actor(std::string path);
        SDL_Rect get_rect() const;
        void update(float delta);
        void handle_collision(const SDL_Rect& collider);
        void render(const vec2& camera_offset);

        int image_index;
        Direction facing_direction;
        vec2 position;
        vec2 velocity;
        int animation_frame;
    private:
        void update_sprite(float delta);

        float animation_timer;
};

