#pragma once

#include "vector.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

typedef struct DialogLine {
    std::string speaker;
    std::string text;
} DialogLine;

typedef enum Direction {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
} Direction;

Direction get_direction_from_name(std::string name);

class Actor {
    public:
        typedef struct PathNode {
            vec2 position;
            Direction direction;
            float wait_duration;
        } PathNode;

        Actor(std::string name, std::string image_path_prefix);
        SDL_Rect get_rect() const;
        bool has_target() const;

        void update(float delta);
        void set_velocity_towards(vec2 target_position);
        void set_direction_towards(vec2 target_position);
        void handle_collision(const SDL_Rect& collider);
        void render(const vec2& camera_offset);

        std::string name;

        int image_idle_index;
        int image_walk_index;
        int image_profile_index;
        int image_index;
        bool image_flipped;
        int animation_frame;

        Direction facing_direction;
        vec2 position;
        vec2 velocity;

        std::vector<PathNode> path;
        std::vector<DialogLine> dialog;

        bool in_scene;
        vec2 target;
    private:
        void update_sprite(float delta);

        float animation_timer;

        int path_index;
        float path_wait_timer;
};

