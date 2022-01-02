#pragma once

#include "render.hpp"
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
                Actor(ImageName image_name);
                void update_sprite(float delta);
                void render(vec2 camera_offset);

                ImageName image_name;
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
        ImageName background_image;
};
