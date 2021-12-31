#pragma once

#include <SDL2/SDL.h>

class IState {
    public:
        IState() {
            finished = false;
        }
        virtual void handle_input(SDL_Event e) = 0;
        virtual void update() = 0;
        virtual void render() = 0;
        bool finished;
};
