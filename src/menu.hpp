#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Menu {
    public:
        Menu(SDL_Rect rect);
        void reset();
        void navigate_up();
        void navigate_down();
        std::string select();
        void render();

        std::vector<std::string> labels;
        int padding_left;
        int padding_top;
    private:
        SDL_Rect rect;
        int cursor_index;
        int cursor_image;
};
