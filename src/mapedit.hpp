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
    EDIT_STATE_SAVE,
    EDIT_STATE_LOAD
} EditorState;

typedef enum Tool {
    TOOL_ADD_COLLIDER,
    TOOL_EDIT_COLLIDER
} Tool;

const int MENUBAR_LENGTH = 2;

class MapEditState : public IState {
    public:
        MapEditState();
        ~MapEditState();
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();
    private:
        void handle_mouse_motion(vec2 relative_motion);
        void handle_mouse_left_click();
        void handle_mouse_left_release();
        void handle_keypress(SDL_Keycode keycode);
        void handle_keypress_typing(SDL_Keycode keycode);
        void handle_menubar_action(int action_state, int action_substate);

        void render_menubar();

        SDL_Rect get_pending_collider();

        void set_state(EditorState new_state);
        void new_map();
        void save_map();
        void load_map();

        vec2 mouse_position;
        vec2 drag_origin;
        bool right_mouse_held;
        bool left_mouse_held;

        bool adding_collider;
        int edit_collider_index;

        std::string menubar_labels[MENUBAR_LENGTH];
        SDL_Rect menubar_borders[MENUBAR_LENGTH];
        std::vector<std::string> menubar_submenu_labels[MENUBAR_LENGTH];
        std::vector<SDL_Rect> menubar_submenu_borders[MENUBAR_LENGTH];
        int menubar_state;

        EditorState state;
        std::string text_input_prompt;
        std::string text_input;

        Tool selected_tool;

        Map* map;
};
