#include "mapedit.hpp"

#include "render.hpp"
#include <fstream>
#include <iostream>

inline bool file_exists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.good();
}

MapEditState::MapEditState() {
    state = EDIT_STATE_DEFAULT;
    text_input_prompt = "";
    text_input = "";
    map = nullptr;
}

MapEditState::~MapEditState() {
    delete map;
}

void MapEditState::handle_input(SDL_Event e) {
    if(state == EDIT_STATE_DEFAULT) {
        if(e.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = e.key.keysym.sym;

            if(keycode == SDLK_n) {
                set_state(EDIT_STATE_NEW);
            }
        }
    } else if(state == EDIT_STATE_NEW) {
        if(e.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = e.key.keysym.sym;

            if(keycode == SDLK_ESCAPE) {
                set_state(EDIT_STATE_DEFAULT);
            } else if(keycode == SDLK_BACKSPACE) {
                if(!text_input.empty()) {
                    text_input.pop_back();
                }
            } else if( (keycode >= SDLK_a && keycode <= SDLK_z)
                    || (keycode >= SDLK_0 && keycode <= SDLK_9)
                    || keycode == SDLK_PERIOD || keycode == SDLK_SLASH) {
                text_input += (char)keycode;
            } else if(keycode == SDLK_MINUS) {
                text_input += '_';
            } else if(keycode == SDLK_RETURN) {
                open_new_map();
            }
        }
    }
}

void MapEditState::update() {
}

void MapEditState::render() {
    if(state == EDIT_STATE_NEW) {
        render_text(text_input_prompt.c_str(), COLOR_WHITE, RENDER_POSITION_CENTERED, 100);
        if(!text_input.empty()) {
            render_text(text_input.c_str(), COLOR_WHITE, RENDER_POSITION_CENTERED, RENDER_POSITION_CENTERED);
        }
    } else if(state == EDIT_STATE_DEFAULT) {
        if(map != nullptr) {
            map->render();
        }
    }
}

void MapEditState::set_state(EditorState new_state) {
    if(new_state == EDIT_STATE_NEW) {
        text_input_prompt = "Please enter the image to use for the map background:";
        text_input = "";
    }

    state = new_state;
}

void MapEditState::open_new_map() {
    if(!file_exists(text_input)) {
        text_input_prompt = "That file doesn't exist! Please try again:";
        return;
    }

    map = new Map(text_input);
    set_state(EDIT_STATE_DEFAULT);
}
