#pragma once

#include <SDL2/SDL.h>

class IState {
    public:
        IState() {
            finished = false;
            render_previous = false;
            new_state = nullptr;
        }
        virtual void handle_input(SDL_Event e) = 0;
        virtual void update(float delta) = 0;
        virtual void render() = 0;
        bool finished;
        bool render_previous;
        IState* new_state;
};
