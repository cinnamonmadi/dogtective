#pragma once

#include "state.hpp"
#include "map.hpp"
#include <SDL2/SDL.h>
#include <string>

typedef enum EditorState {
    EDIT_STATE_DEFAULT,
    EDIT_STATE_NEW,
    EDIT_STATE_LOAD
} EditorState;

class MapEditState : public IState {
    public:
        MapEditState();
        ~MapEditState();
        void handle_input(SDL_Event e);
        void update();
        void render();
    private:
        void set_state(EditorState new_state);
        void open_new_map();

        EditorState state;
        std::string text_input_prompt;
        std::string text_input;

        Map* map;
};
