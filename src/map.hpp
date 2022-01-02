#pragma once

#include "vector.hpp"

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
                Actor(const char* image_path);
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
        void update(float delta);
        void render();

        vec2 camera_offset;
        Actor player;
        int background_image;
};
