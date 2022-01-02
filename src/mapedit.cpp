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

inline bool file_exists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return file.good();
}

MapEditState::MapEditState() {
    mouse_position = (vec2) { .x = 0, .y = 0 };
    drag_origin = (vec2) { .x = 0, .y = 0 };
    right_mouse_held = false;
    left_mouse_held = false;
    adding_collider = false;
    edit_collider_index = 0;

    menubar_state = -1;
    menubar_labels[0] = "File";
    menubar_submenu_labels[0].push_back("New");
    menubar_submenu_labels[0].push_back("Load");
    menubar_submenu_labels[0].push_back("Save");
    menubar_submenu_labels[0].push_back("Exit");
    menubar_labels[1] = "Tool";
    menubar_submenu_labels[1].push_back("Add Colliders");
    menubar_submenu_labels[1].push_back("Edit Colliders");

    for(int i = 0; i < MENUBAR_LENGTH; i++) {
        for(int j = 0; j < menubar_submenu_labels[i].size(); j++) {
            menubar_submenu_borders[i].push_back((SDL_Rect) { .x = 0, .y = 0, .w = 0, .h = 0 });
        }
    }

    state = EDIT_STATE_DEFAULT;
    text_input_prompt = "";
    text_input = "";

    selected_tool = TOOL_ADD_COLLIDER;

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
            left_mouse_held = true;
        }
        drag_origin = mouse_position;
    } else if(e.type == SDL_MOUSEBUTTONUP) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            right_mouse_held = false;
        } else if(e.button.button == SDL_BUTTON_LEFT) {
            handle_mouse_left_release();
            left_mouse_held = false;
        }
    } else if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        if(state == EDIT_STATE_DEFAULT) {
            handle_keypress(keycode);
        } else {
            handle_keypress_typing(keycode);
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

    if(selected_tool == TOOL_ADD_COLLIDER) {
        adding_collider = true;
    } else if(selected_tool == TOOL_EDIT_COLLIDER) {
        for(int i = 0; i < map->colliders.size(); i++) {
            if(vec2_in_rect(mouse_position, map->colliders[i])) {
                edit_collider_index = i;
                return;
            }
        }
        edit_collider_index = -1;
    }

    menubar_state = -1;
}

void MapEditState::handle_mouse_left_release() {
    if(state != EDIT_STATE_DEFAULT)  {
        return;
    }

    if(adding_collider && map != nullptr) {
        map->colliders.push_back(get_pending_collider());
        selected_tool = TOOL_EDIT_COLLIDER;
        edit_collider_index = map->colliders.size() - 1;
    }
    adding_collider = false;
}

void MapEditState::handle_keypress(SDL_Keycode keycode) {
    if(selected_tool == TOOL_EDIT_COLLIDER && edit_collider_index >= 0 && edit_collider_index < map->colliders.size()) {
        switch(keycode) {
            case SDLK_BACKSPACE:
                map->colliders.erase(map->colliders.begin() + edit_collider_index);
                edit_collider_index = -1;
                break;
            case SDLK_w:
                map->colliders[edit_collider_index].y--;
                break;
            case SDLK_s:
                map->colliders[edit_collider_index].y++;
                break;
            case SDLK_d:
                map->colliders[edit_collider_index].x++;
                break;
            case SDLK_a:
                map->colliders[edit_collider_index].x--;
                break;
            case SDLK_e:
                if(map->colliders[edit_collider_index].w > 1) {
                    map->colliders[edit_collider_index].w--;
                }
                break;
            case SDLK_r:
                map->colliders[edit_collider_index].w++;
                break;
            case SDLK_f:
                if(map->colliders[edit_collider_index].h > 1) {
                    map->colliders[edit_collider_index].h--;
                }
                break;
            case SDLK_g:
                map->colliders[edit_collider_index].h++;
                break;
        }
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
        if(state == EDIT_STATE_NEW) {
            new_map();
        } else if(state == EDIT_STATE_SAVE) {
            save_map();
        } else if(state == EDIT_STATE_LOAD) {
            load_map();
        }
    }
}

void MapEditState::handle_menubar_action(int action_state, int action_substate) {
    if(action_state == MENU_INDEX_FILE && action_substate == SUBMENU_INDEX_NEW) {
        set_state(EDIT_STATE_NEW);
    } else if(action_state == MENU_INDEX_FILE && action_substate == SUBMENU_INDEX_SAVE) {
        set_state(EDIT_STATE_SAVE);
    } else if(action_state == MENU_INDEX_FILE && action_substate == SUBMENU_INDEX_LOAD) {
        set_state(EDIT_STATE_LOAD);
    } else if(action_state == MENU_INDEX_FILE && action_substate == SUBMENU_INDEX_EXIT) {
        finished = true;
    } else if(action_state == MENU_INDEX_TOOL) {
        selected_tool = (Tool)action_substate;
    }
}

void MapEditState::update(float delta) {
}

void MapEditState::render() {
    if(state == EDIT_STATE_NEW || state == EDIT_STATE_LOAD || state == EDIT_STATE_SAVE) {
        render_text(text_input_prompt.c_str(), COLOR_WHITE, (vec2){ .x = RENDER_POSITION_CENTERED, .y = 100});
        if(!text_input.empty()) {
            render_text(text_input.c_str(), COLOR_WHITE, (vec2){ .x = RENDER_POSITION_CENTERED, .y = RENDER_POSITION_CENTERED});
        }
    } else if(state == EDIT_STATE_DEFAULT) {
        if(map != nullptr) {
            map->render();
            for(int i = 0; i < map->colliders.size(); i++) {
                if(selected_tool == TOOL_EDIT_COLLIDER && i == edit_collider_index) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }

                SDL_Rect collider_rendered_rect = map->colliders[i];
                collider_rendered_rect.x -= map->camera_offset.x;
                collider_rendered_rect.y -= map->camera_offset.y;

                SDL_RenderFillRect(renderer, &collider_rendered_rect);
            }
        }
        if(adding_collider) {
            SDL_Rect pending_collider_rect = get_pending_collider();
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pending_collider_rect);
        }
        render_menubar();
        render_text(("Selected tool: " + menubar_submenu_labels[1][selected_tool]).c_str(), COLOR_WHITE, (vec2) { .x = 0, .y = 350 });
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

SDL_Rect MapEditState::get_pending_collider() {
    return (SDL_Rect) {
        .x = (int)fmin(drag_origin.x, mouse_position.x),
        .y = (int)fmin(drag_origin.y, mouse_position.y),
        .w = abs(drag_origin.x - mouse_position.x),
        .h = abs(drag_origin.y - mouse_position.y)
    };
}

void MapEditState::set_state(EditorState new_state) {
    if(new_state == EDIT_STATE_NEW) {
        text_input_prompt = "Please enter the image to use for the map background:";
        text_input = "";
    } else if(new_state == EDIT_STATE_SAVE) {
        text_input_prompt = "Enter the file path to save to:";
        text_input = "";
    } else if(new_state == EDIT_STATE_LOAD) {
        text_input_prompt = "Enter the file path to load from:";
        text_input = "";
    }

    state = new_state;
}

void MapEditState::new_map() {
    if(!file_exists(text_input)) {
        text_input_prompt = "That file doesn't exist! Please try again:";
        return;
    }

    map = new Map();
    map->background_image = render_load_image(text_input);
    set_state(EDIT_STATE_DEFAULT);
}

void MapEditState::save_map() {
    map->save(text_input);
    set_state(EDIT_STATE_DEFAULT);
}

void MapEditState::load_map() {
    if(!file_exists(text_input)) {
        text_input_prompt = "That file doesn't exist! Please try again:";
        return;
    }

    map = new Map();
    map->load(text_input);
    set_state(EDIT_STATE_DEFAULT);
}
