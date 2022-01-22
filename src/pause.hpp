#pragma once

#include "state.hpp"
#include "menu.hpp"
#include <SDL2/SDL.h>
#include <vector>

typedef enum PauseMenuState {
    PAUSE_MENU_MAIN,
    PAUSE_MENU_EVIDENCE
} PauseMenuState;

class Pause : public IState {
    public:
        Pause();
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();
    private:
        void open_evidence_menu();

        void menu_select();
        void menu_back();

        PauseMenuState get_state() const;
        Menu* get_current_menu();

        std::vector<Menu> menus;
};
