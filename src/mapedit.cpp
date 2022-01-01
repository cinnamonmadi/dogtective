#include "mapedit.hpp"

#include "render.hpp"
#include <fstream>
#include <iostream>

const int MENUBAR_PADDING = 10;
const int MENUBAR_HEIGHT = 12;

// These enums are to make the code look more readable in handle_menubar_action()
enum MENU_INDECES {
    MENU_INDEX_FILE,
    MENU_INDEX_TOOL
};
enum FILE_MENU_INDECES {
    SUBMENU_INDEX_NEW,
    SUBMENU_INDEX_LOAD,
    SUBMENU_INDEX_SAVE,
    SUBMENU_INDEX_EXIT
};
enum TOOL_MENU_INDECES {
    SUBMENU_INDEX_COLLIDERS,
    SUBMENU_INDEX_NPCS,
};

inline bool file_exists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.good();
}

MapEditState::MapEditState() {
    mouse_position = (vec2) { .x = 0, .y = 0 };
    right_mouse_held = false;

    menubar_state = -1;
    menubar_labels[0] = "File";
    menubar_submenu_labels[0].push_back("New");
    menubar_submenu_labels[0].push_back("Load");
    menubar_submenu_labels[0].push_back("Save");
    menubar_submenu_labels[0].push_back("Exit");
    menubar_labels[1] = "Tool";
    menubar_submenu_labels[1].push_back("Colliders");
    menubar_submenu_labels[1].push_back("NPCs");

    for(int i = 0; i < MENUBAR_LENGTH; i++) {
        for(int j = 0; j < menubar_submenu_labels[i].size(); j++) {
            menubar_submenu_borders[i].push_back((SDL_Rect) { .x = 0, .y = 0, .w = 0, .h = 0 });
        }
    }

    state = EDIT_STATE_DEFAULT;
    text_input_prompt = "";
    text_input = "";

    map = nullptr;
}

MapEditState::~MapEditState() {
    delete map;
}

void MapEditState::handle_input(SDL_Event e) {
    if(e.type == SDL_MOUSEMOTION) {
        mouse_position = (vec2) { .x = e.motion.x, .y = e.motion.y };
        handle_mouse_motion((vec2){ .x = e.motion.xrel, .y = e.motion.yrel });
    } else if(e.type == SDL_MOUSEBUTTONDOWN) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            right_mouse_held = true;
        } else if(e.button.button == SDL_BUTTON_LEFT) {
            handle_mouse_left_click();
        }
    } else if(e.type == SDL_MOUSEBUTTONUP) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            right_mouse_held = false;
        }
    } else if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(state) {
            case EDIT_STATE_DEFAULT:
                handle_keypress(keycode);
                break;
            case EDIT_STATE_NEW:
                handle_keypress_typing(keycode);
                break;
            default:
                break;
        }
    }
}

void MapEditState::handle_mouse_motion(vec2 relative_motion) {
    if(state == EDIT_STATE_DEFAULT && map != nullptr && right_mouse_held) {
        map->camera_offset -= relative_motion;
    }
}

void MapEditState::handle_mouse_left_click() {
    if(state != EDIT_STATE_DEFAULT) {
        return;
    }

    for(int i = 0; i < MENUBAR_LENGTH; i++) {
        if(vec2_in_rect(mouse_position, menubar_borders[i])) {
            if(menubar_state == i) {
                menubar_state = -1;
            } else {
                menubar_state = i;
            }
            return;
        }
    }

    if(menubar_state != -1) {
        for(int i = 0; i < menubar_submenu_labels[menubar_state].size(); i++) {
            if(vec2_in_rect(mouse_position, menubar_submenu_borders[menubar_state][i])) {
                handle_menubar_action(menubar_state, i);
                menubar_state = -1;
                return;
            }
        }
    }

    menubar_state = -1;
}

void MapEditState::handle_keypress(SDL_Keycode keycode) {
    if(keycode == SDLK_n) {
        set_state(EDIT_STATE_NEW);
    }
}

void MapEditState::handle_keypress_typing(SDL_Keycode keycode) {
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

void MapEditState::handle_menubar_action(int action_state, int action_substate) {
    if(action_state == MENU_INDEX_FILE && action_substate == SUBMENU_INDEX_NEW) {
        set_state(EDIT_STATE_NEW);
    }
}

void MapEditState::update() {
}

void MapEditState::render() {
    if(state == EDIT_STATE_NEW) {
        render_text(text_input_prompt.c_str(), COLOR_WHITE, (vec2){ .x = RENDER_POSITION_CENTERED, .y = 100});
        if(!text_input.empty()) {
            render_text(text_input.c_str(), COLOR_WHITE, (vec2){ .x = RENDER_POSITION_CENTERED, .y = RENDER_POSITION_CENTERED});
        }
    } else if(state == EDIT_STATE_DEFAULT) {
        if(map != nullptr) {
            map->render();
        }
        render_menubar();
    }
}

void MapEditState::render_menubar() {
    int menubar_render_x = 0;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < MENUBAR_LENGTH; i++) {
        SDL_Color text_color = COLOR_WHITE;
        if(menubar_state == i) {
            text_color = COLOR_BLACK;
        }
        Image* text_image = render_create_text_image(menubar_labels[i].c_str(), text_color);

        menubar_borders[i] = (SDL_Rect){
            .x = menubar_render_x,
            .y = 0,
            .w = text_image->size.x + (MENUBAR_PADDING * 2),
            .h = MENUBAR_HEIGHT
        };
        if(menubar_state == i) {
            SDL_RenderFillRect(renderer, &menubar_borders[i]);
        } else {
            SDL_RenderDrawRect(renderer, &menubar_borders[i]);
        }

        SDL_Rect src_rect = (SDL_Rect) { .x = 0, .y = 0, .w = text_image->size.x, .h = text_image->size.y };
        SDL_Rect dst_rect = (SDL_Rect) { .x = menubar_render_x + MENUBAR_PADDING, .y = 0, .w = text_image->size.x, .h = text_image->size.y };

        SDL_RenderCopy(renderer, text_image->texture, &src_rect, &dst_rect);
        SDL_DestroyTexture(text_image->texture);
        delete text_image;

        menubar_render_x += menubar_borders[i].w;
    }

    if(menubar_state != -1) {
        menubar_render_x = menubar_borders[menubar_state].x;
        for(int i = 0; i < menubar_submenu_labels[menubar_state].size(); i++) {
            Image* text_image = render_create_text_image(menubar_submenu_labels[menubar_state].at(i).c_str(), COLOR_WHITE);

            menubar_submenu_borders[menubar_state][i] = (SDL_Rect) {
                .x = menubar_render_x,
                .y = MENUBAR_HEIGHT * (i + 1),
                .w = text_image->size.x + (MENUBAR_PADDING * 2),
                .h = MENUBAR_HEIGHT
            };
            SDL_RenderDrawRect(renderer, &menubar_submenu_borders[menubar_state][i]);

            SDL_Rect src_rect = (SDL_Rect) { .x = 0, .y = 0, .w = text_image->size.x, .h = text_image->size.y };
            SDL_Rect dst_rect = (SDL_Rect) { .x = menubar_render_x + MENUBAR_PADDING, .y = menubar_submenu_borders[menubar_state][i].y, .w = text_image->size.x, .h = text_image->size.y };

            SDL_RenderCopy(renderer, text_image->texture, &src_rect, &dst_rect);
            SDL_DestroyTexture(text_image->texture);
            delete text_image;
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
