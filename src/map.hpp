#pragma once

#include "vector.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

typedef enum Direction {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
} Direction;

class Map {
    public:
        class Actor {
            public:
                Actor();
                Actor(std::string image_path);
                SDL_Rect get_rect() const;

                void update_sprite(float delta);
                void render(vec2 camera_offset);

                int image_index;
                Direction facing_direction;
                vec2 position;
                vec2 velocity;
                int animation_frame;
            private:
                float animation_timer;
        };

        Map();
        bool load(std::string path);
        bool save(std::string path);

        void update(float delta);
        void actor_handle_collisions(Actor& actor);

        void render();

        int background_image;
        std::vector<SDL_Rect> colliders;

        vec2 camera_offset;
        Actor player;
};
