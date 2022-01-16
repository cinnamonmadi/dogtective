#pragma once

#include "state.hpp"
#include "menu.hpp"
#include <SDL2/SDL.h>

class Pause : public IState {
    public:
        Pause();
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();
    private:
        void open_menu_evidence();

        std::vector<Menu> menus;
};
