#pragma once

#include "state.hpp"
#include "actor.hpp"
#include "vector.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Map : public IState {
    public:
        Map(std::string path);
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();

    private:
        bool direction_key_pressed[4];
        vec2 player_direction;
        vec2 camera_offset;

        int background_image;
        std::vector<SDL_Rect> colliders;

        std::vector<Actor> actors;
        int actor_player;
};
