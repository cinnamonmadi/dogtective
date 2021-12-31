#pragma once

#include "state.hpp"

class MapState : public IState {
    public:
        void handle_input(SDL_Event e);
        void update();
        void render();
};
