#pragma once

#include "state.hpp"
#include "vector.hpp"
#include "map.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

typedef enum EditorState {
    EDIT_STATE_DEFAULT,
    EDIT_STATE_NEW,
    EDIT_STATE_LOAD
} EditorState;

const int MENUBAR_LENGTH = 2;

class MapEditState : public IState {
    public:
        MapEditState();
        ~MapEditState();
        void handle_input(SDL_Event e);
        void update();
        void render();
    private:
        void handle_mouse_motion(vec2 relative_motion);
        void handle_mouse_left_click();
        void handle_keypress(SDL_Keycode keycode);
        void handle_keypress_typing(SDL_Keycode keycode);
        void handle_menubar_action(int action_state, int action_substate);

        void render_menubar();

        void set_state(EditorState new_state);
        void open_new_map();

        vec2 mouse_position;
        bool right_mouse_held;

        std::string menubar_labels[MENUBAR_LENGTH];
        SDL_Rect menubar_borders[MENUBAR_LENGTH];
        std::vector<std::string> menubar_submenu_labels[MENUBAR_LENGTH];
        std::vector<SDL_Rect> menubar_submenu_borders[MENUBAR_LENGTH];
        int menubar_state;

        EditorState state;
        std::string text_input_prompt;
        std::string text_input;

        Map* map;
};
