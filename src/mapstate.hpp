#pragma once

#include "state.hpp"
#include "map.hpp"
#include "vector.hpp"

class MapState : public IState {
    public:
        MapState();
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();
    private:
        Map map;
        vec2 player_direction;
        bool direction_key_pressed[4];
};
