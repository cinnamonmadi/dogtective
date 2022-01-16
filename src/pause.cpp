#include "pause.hpp"

#include "render.hpp"
#include "vector.hpp"
#include "inventory.hpp"
#include <string>

const int MAIN_MENU_LENGTH = 3;
const std::string MAIN_MENU_LABELS[MAIN_MENU_LENGTH] = {"Evidence", "Save", "Quit"};

Pause::Pause() {
    render_previous = true;

    menus.push_back(Menu((SDL_Rect) {
        .x = SCREEN_WIDTH - 100,
        .y = 0,
        .w = 100,
        .h = SCREEN_HEIGHT
    }));
    menus[0].labels.push_back("Evidence");
    menus[0].labels.push_back("Save");
    menus[0].labels.push_back("Quit");
}

void Pause::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(keycode) {
            case SDLK_w: {
                menus[menus.size() - 1].navigate_up();
                break;
            }
            case SDLK_s: {
                menus[menus.size() - 1].navigate_down();
                break;
            }
            case SDLK_SPACE: {
                std::string action = menus[menus.size() - 1].select();
                if(menus.size() == 1 && action == "Evidence") {
                    open_menu_evidence();
                }
                break;
            }
            case SDLK_x: {
                if(menus.size() == 1) {
                    finished = true;
                } else {
                    menus.pop_back();
                }
                break;
            }
            case SDLK_p: {
                finished = true;
                break;
            }
        }
    }
}

void Pause::open_menu_evidence() {
    menus.push_back(Menu((SDL_Rect) {
        .x = SCREEN_WIDTH - 200,
        .y = 0,
        .w = 200,
        .h = SCREEN_HEIGHT
    }));
    for(int i = 0; i < evidence.size(); i++) {
        menus[menus.size() - 1].labels.push_back(evidence[i].name);
    }
}

void Pause::update(float delta) {

}

void Pause::render() {
    menus[menus.size() - 1].render();
}
