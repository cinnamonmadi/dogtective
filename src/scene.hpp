#pragma once

#include "state.hpp"
#include "actor.hpp"
#include "vector.hpp"
#include "script.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Scene : public IState {
    public:
        Scene(std::string path);
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();

    private:
        int get_actor_from_name(std::string name);
        bool script_execute(int script_index, float delta);

        bool direction_key_pressed[4];
        vec2 player_direction;
        vec2 camera_offset;

        int background_image;
        std::vector<SDL_Rect> colliders;

        std::vector<Actor> actors;
        int actor_player;

        std::vector<Script> scripts;
};
